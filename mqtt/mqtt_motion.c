#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <czmq.h>   // czmq
#include <mosquitto.h>

// MQTT 
#define MQTT_HOST       "127.0.0.1"
#define MQTT_PORT       1883
#define MQTT_Motion     "/sensors/motion"

static void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
    printf("connect callback, rc=%d\n", result);
}

int main(int argc, char *argv[]) 
{
    // read zmq endpoint, mqtt ip & port from console
    char *end_point = (argc > 1) ? argv[1] : "ipc:///tmp/dummy";
    char * mqtt_ip  = (argc > 2) ? argv[2] : MQTT_HOST;
    int mqtt_port   = (argc > 3) ? atoi(argv[3]) : MQTT_PORT;
    
    int status;

    // init mqtt lib
    struct mosquitto *mosq;
    char clientid[24];
    char json[60]; // json string
    mosquitto_lib_init();
    memset(clientid, 0, 24);
    snprintf(clientid, 23, "leapmotion_motion_%d", getpid());
    mosq = mosquitto_new(clientid, true, 0);

    if (mosq) 
    {
        // connect to mqtt broker
        mosquitto_connect_callback_set(mosq, connect_callback);
        status = mosquitto_connect(mosq, mqtt_ip, mqtt_port, 60);

        // zmq
        zctx_t *context = zctx_new ();
        void *subscriber = zsocket_new (context, ZMQ_SUB);
        zsocket_connect (subscriber, end_point);
        zsocket_set_subscribe (subscriber, ""); // set filter

        printf("start motion listener\n");
        
        while (!zctx_interrupted) // handle ctrl+c
        { 
            zmsg_t *msg = zmsg_recv(subscriber);
            if (msg != NULL)
            {
                zframe_t *x = zmsg_pop(msg);
                zframe_t *y = zmsg_pop(msg);
                char *x_buffer = zframe_strdup(x);
                char *y_buffer = zframe_strdup(y);

                memset(json, 0, 60);
                snprintf(json, 59, "{\"x\":%.4f,\"y\":%.4f}", atof(x_buffer), atof(y_buffer));
                status = mosquitto_loop(mosq, -1, 1);

                if (status)
                {
                    printf("mqtt connection error!\n");
                    sleep(3);
                    mosquitto_reconnect(mosq);
                }
                else
                {
                    printf("mqtt pub:(%.4f, %.4f)\n", atof(x_buffer), atof(y_buffer));
                    mosquitto_publish(mosq, NULL, MQTT_Motion, strlen(json), json, 2, true);   
                }

                // cleanup
                zframe_destroy(&x);
                zframe_destroy(&y);
                zmsg_destroy(&msg);
                //usleep(100000); // sleep 0.1 secs
            }
        }
        status = 0;
        zctx_destroy(&context);
    }
    else
    {
        status = -1;
        printf("init mqtt client failed\n");
    }
    // clean up section
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    exit(status);
}