#include <gst/gst.h>  
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>


static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = (GMainLoop*)data;

	switch (GST_MESSAGE_TYPE (msg))
	{
	case GST_MESSAGE_EOS:
		g_print ("End-of-stream\n");	
		break;	
	case GST_MESSAGE_ERROR: //normal debug callback
		{
			gchar *debug = NULL;
			GError *err = NULL;

			gst_message_parse_error (msg, &err, &debug);

			g_print ("Error: %s\n", err->message);
			g_error_free (err);

			if (debug)
			{
				g_print ("Debug deails: %s\n", debug);
				g_free (debug);
			}

			g_main_loop_quit (loop);
			break;
		}
	default:
		break;
	}

	return TRUE;
}

int main(int argc, char *argv[]) {
	
	
	const char *arg1_gst[]  = {"gstvideo"};
	const char *arg2_gst[]  = {"--gst-disable-registry-update"};
	const char *arg3_gst[]  = {"--gst-debug-level=9"};
	char ** argv_gst[3] = {(char **)arg1_gst,(char **)arg2_gst,(char **)arg3_gst};
	int argc_gst = 3;
	/* Initialize GStreamer */
	gst_init (&argc_gst, argv_gst );

	GMainLoop *loop = g_main_loop_new (NULL, FALSE);

	GstPipeline *pipeline = GST_PIPELINE (gst_parse_launch((char *)"videotestsrc pattern=black ! video/x-raw,width=320,height=240,framerate=10/1 ! queue ! videoin. "
	"videotestsrc ! video/x-raw,width=640,height=480,framerate=10/1 ! queue ! videorate ! "
	"video/x-raw,framrate=10/1 ! videoscale ! video/x-raw,width=320,height=240 ! queue ! videoin. "
	"input-selector name=videoin ! textoverlay vertical-render=true  shaded-background=true valignment=center line-alignment=center halignment=center font-desc=\"DejaVu Sans Mono,48\" name=textinput "
	"! videoflip method=2 ! videoconvert ! fakesink", NULL));

	
	GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);
	
	
	gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING);	

	g_main_loop_run (loop); //let gstreamer's GLib event loop take over
	
	return 0;
}
