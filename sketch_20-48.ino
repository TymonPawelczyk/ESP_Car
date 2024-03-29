#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define STOP 0

#define FRONT_RIGHT_MOTOR 0
#define BACK_RIGHT_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define BACK_LEFT_MOTOR 3

#define FORWARD 1
#define BACKWARD -1

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;    
};


const char* ssid     = "MyWiFiCar";
const char* password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0 ,maximum-scale=1.0, user-scalable=no" />
    <title>Esp Car
    </title>
    <style>
        *{
            padding: 0;
            margin: 0;
        }
        body{
            background-color: #ffffff;
        }
        .container{
            display: grid;
            background-color: #dee2e6;
            grid-template-columns: 1fr 4fr 4fr 1fr;
            grid-template-rows: 10vh 85vh 5vh;
            grid-template-areas: 
            "title title title title"
            ". main main . "
            " footer footer footer footer";
        }
        .title{
            display: grid;
            background-color: #adb5bd;
            grid-area: title;
            text-align: center;
            
        }
        .t{
            place-self: center;
            color: #252422;
        }
        .main{
            display: grid;
            grid-area: main;
            align-items: center;
            place-self: center;
            
        }
        .wrapperSter{
            display: grid;
            grid-template-columns: 1fr 2fr 1fr;
            grid-template-rows: 1fr 1fr;
            grid-template-areas: 
            ". arrowUp ."
            "arrowLeft arrowDown arrowRight";
            column-gap: 5px;
            row-gap: 5px;
        }
        .footer{
            grid-area: footer;
            background-color: #ced4da;
            color: #252422;
            font-size: xx-small;
            
        }
        .arrowUp{
            grid-area: arrowUp;
            justify-self: center;

        }
        .arrowDown{
            grid-area: arrowDown;
            justify-self: center;

        }
        .arrowLeft{
            grid-area: arrowLeft;
            justify-self: center;
        }
        .arrowRight{
            grid-area: arrowRight;
            justify-self: center;
        }
        .box-btn{
            display: grid;
            background-color: #003049;
            color: #d62828;
            width: 90px;
            height: 90px;
            border-radius: 10px;
        }
        .btn{
            place-self: center;
            text-align: center;
        }
        .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }
    </style>
</head>
<body class="noselect">
    <div class="container">
        <div class="title">
            <div class="t">
                🚨🔥🏎🚀🔥🚨
            </div>
        </div>
        <div class="main">
            <div class="wrapperSter">
                <div class="arrowUp">
                    <div class="box-btn" ontouchstart='onTouchStartAndEnd("1")' ontouchend='onTouchStartAndEnd("0")'>
                        <div class="btn">
                            <span class="arrows" >&#8679;</span>
                        </div>
                    </div>
                </div>
                <div class="arrowLeft">
                    <div class="box-btn" ontouchstart='onTouchStartAndEnd("3")' ontouchend='onTouchStartAndEnd("0")'>
                    <div class="btn">
                        &#8678;
                    </div>
                </div></div>
                <div class="arrowDown">
                    <div class="box-btn" ontouchstart='onTouchStartAndEnd("2")' ontouchend='onTouchStartAndEnd("0")'>
                    <div class="btn">
                        &#8681;
                    </div>
                </div></div>
                <div class="arrowRight">
                    <div class="box-btn" ontouchstart='onTouchStartAndEnd("4")' ontouchend='onTouchStartAndEnd("0")'>
                    <div class="btn">
                        &#8680;
                    </div>
                </div></div>
            </div>
        </div>
        <div class="footer">Lorem ipsum dolor sit amet.</div>
    </div>
    <script>
      var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
      var websocket;
      
      function initWebSocket() 
      {
        websocket = new WebSocket(webSocketUrl);
        websocket.onopen    = function(event){};
        websocket.onclose   = function(event){setTimeout(initWebSocket, 2000);};
        websocket.onmessage = function(event){};
      }
      function onTouchStartAndEnd(value) 
      {
        websocket.send(value);
      }
          
      window.onload = initWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
    
  </body>
</html> 
)HTMLHOMEPAGE";


void rotateMotor(int motorNumber, int motorDirection)
{
  
}
void processCarMovement(String inputValue)
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());  
  switch(inputValue.toInt())
  {

    case UP:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);                  
      break;
  
    case DOWN:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
      break;
  
    case LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
      break;
  
    case RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
      break;
  
  
    case STOP:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  
    default:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}


void onWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        processCarMovement(myData.c_str());       
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}




void setup(void) 
{
  
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  ws.cleanupClients(); 
}