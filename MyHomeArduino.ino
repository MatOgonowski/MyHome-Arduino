#include <ArduinoJson.h>
#include <stdio.h>
#include <DS1302.h>

namespace {
const int kCePin   = 11;  // Chip Enable // RST
const int kIoPin   = 12;  // Input/Output // DAT
const int kSclkPin = 13;  // Serial Clock // CLK

DS1302 rtc(kCePin, kIoPin, kSclkPin);
}

struct Day {
  bool state;
  int id;
  int timeTo;
  int timeFrom;
};

/*
   struktura opisujaca pomieszczenia
*/
struct Room {
  bool state;
  String roomName;   //zmienic nazwe ze stringa na char* oraz zamiane int na char*
  int intensity;
  bool autoOn;
  int blackout;
  int pin;
  int lightSensor;
  Day dayList[7];
};


/*
   zdefiniowane stale wykorzystywane w programie
*/
#define ROOM_LIST_SIZE 4
#define TIMEOUT 100
#define ROOM_LIST_REQUEST 0
#define ROOM_STATE_REQUEST 1
#define CHANGE_STATE 2
#define CHANGE_NAME 3
#define CHANGE_INTENSITY 4
#define CHANGE_AUTO_ON 5
#define CHANGE_BLACKOUT 6
#define OFF_ALL 7
#define SIMULATION_STATE 8
#define CHANGE_DAY_STATE 9
#define CHANGE_TIME_FROM 10
#define CHANGE_TIME_TO 11
#define ROOM_DAYS_REQUEST 12
#define CHANGE_SIMULATION_STATE 13

#define JSON_BUFFER_RECEIVED_MESSAGE JSON_OBJECT_SIZE(4) + 50
#define JSON_BUFFER_ROOM_STATE JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5)+ 100
#define JSON_BUFFER_ROOM_LIST JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 4*JSON_OBJECT_SIZE(2) + 150
#define JSON_BUFFER_DAY_LIST JSON_ARRAY_SIZE(7) + JSON_OBJECT_SIZE(1) + 7*JSON_OBJECT_SIZE(4)
#define JSON_BUFFER_SIMULATION_STATE JSON_OBJECT_SIZE(1)

const int ROOM0 = 3;
const int ROOM1 = 5;
const int ROOM2 = 6;
const int ROOM3 = 9;

Room roomList[ROOM_LIST_SIZE];
Day dayList[7];
bool simulationState = false;

void makeDayList(Room &roomList) {
  dayList[7];
  for (int i = 0; i < 7; i++) {
    roomList.dayList[i].state = false;
    roomList.dayList[i].id = i + 1;
    roomList.dayList[i].timeFrom = 0;
    roomList.dayList[i].timeTo = 0;
  }
}

/*
   inicjalizacje wszystkich wejsc/wyjsc
   oraz wprowadzenie podstawowch danych dla kazdego pomieszczenia
*/
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  pinMode(ROOM0, OUTPUT);
  pinMode(ROOM1, OUTPUT);
  pinMode(ROOM2, OUTPUT);
  pinMode(ROOM3, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  for (int i = 0 ; i < ROOM_LIST_SIZE ; i++) {
    String nname = "room" + String(i);
    roomList[i].state = false;
    roomList[i].roomName = nname;
    roomList[i].intensity = 255;
    roomList[i].autoOn = false;
    roomList[i].blackout = 0;
    makeDayList(roomList[i]);

  }
  roomList[0].pin = ROOM0;
  roomList[1].pin = ROOM1;
  roomList[2].pin = ROOM2;
  roomList[3].pin = ROOM3;
}

void sendRoomList() {
  StaticJsonBuffer<JSON_BUFFER_ROOM_LIST> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& room_list = root.createNestedArray("roomList");
  JsonObject& room_list_0 = room_list.createNestedObject();
  room_list_0["id"] = 0;
  room_list_0["name"] = roomList[0].roomName;

  JsonObject& room_list_1 = room_list.createNestedObject();
  room_list_1["id"] = 1;
  room_list_1["name"] = roomList[1].roomName;

  JsonObject& room_list_2 = room_list.createNestedObject();
  room_list_2["id"] = 2;
  room_list_2["name"] = roomList[2].roomName;

  JsonObject& room_list_3 = room_list.createNestedObject();
  room_list_3["id"] = 3;
  room_list_3["name"] = roomList[3].roomName;

  root.printTo(Serial);
  Serial.println();
}

void sendRoomState(int i) {
  StaticJsonBuffer<JSON_BUFFER_ROOM_STATE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& room = root.createNestedObject("room");
  room["state"] = roomList[i].state;
  room["roomName"] = roomList[i].roomName;
  room["intensity"] = map(roomList[i].intensity, 0, 255, 0, 100);
  room["autoOn"] = roomList[i].autoOn;
  room["blackout"] = map(roomList[i].blackout , 0, 255, 0, 100);
  root.printTo(Serial);
  Serial.println();
}

void sendDaysList(int i) {
  StaticJsonBuffer<JSON_BUFFER_DAY_LIST> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& room = root.createNestedArray("room");

  JsonObject& room_0 = room.createNestedObject();
  room_0["dayId"] = roomList[i].dayList[0].id;
  room_0["state"] = roomList[i].dayList[0].state;
  room_0["timeFrom"] = roomList[i].dayList[0].timeFrom;
  room_0["timeTo"] = roomList[i].dayList[0].timeTo;

  JsonObject& room_1 = room.createNestedObject();
  room_1["dayId"] = roomList[i].dayList[1].id;;
  room_1["state"] = roomList[i].dayList[1].state;
  room_1["timeFrom"] = roomList[i].dayList[1].timeFrom;
  room_1["timeTo"] = roomList[i].dayList[1].timeTo;

  JsonObject& room_2 = room.createNestedObject();
  room_2["dayId"] = roomList[i].dayList[2].id;;
  room_2["state"] = roomList[i].dayList[2].state;
  room_2["timeFrom"] = roomList[i].dayList[2].timeFrom;
  room_2["timeTo"] = roomList[i].dayList[2].timeTo;

  JsonObject& room_3 = room.createNestedObject();
  room_3["dayId"] = roomList[i].dayList[3].id;;
  room_3["state"] = roomList[i].dayList[3].state;
  room_3["timeFrom"] = roomList[i].dayList[3].timeFrom;
  room_3["timeTo"] = roomList[i].dayList[3].timeTo;

  JsonObject& room_4 = room.createNestedObject();
  room_4["dayId"] = roomList[i].dayList[4].id;;
  room_4["state"] = roomList[i].dayList[4].state;
  room_4["timeFrom"] = roomList[i].dayList[4].timeFrom;
  room_4["timeTo"] = roomList[i].dayList[4].timeTo;

  JsonObject& room_5 = room.createNestedObject();
  room_5["dayId"] = roomList[i].dayList[5].id;;
  room_5["state"] = roomList[i].dayList[5].state;
  room_5["timeFrom"] = roomList[i].dayList[5].timeFrom;
  room_5["timeTo"] = roomList[i].dayList[5].timeTo;

  JsonObject& room_6 = room.createNestedObject();
  room_6["dayId"] = roomList[i].dayList[6].id;;
  room_6["state"] = roomList[i].dayList[6].state;
  room_6["timeFrom"] = roomList[i].dayList[6].timeFrom;
  room_6["timeTo"] = roomList[i].dayList[6].timeTo;

  root.printTo(Serial);
  Serial.println();
}

void sendSimulationState() {
  StaticJsonBuffer<JSON_BUFFER_SIMULATION_STATE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["state"] = simulationState;

  root.printTo(Serial);
  Serial.println();
}


/*
   odczytywanie wiadomosci otrzymanych z aplikacji
*/
JsonObject& getMsg() {
  StaticJsonBuffer<JSON_BUFFER_RECEIVED_MESSAGE> jsonBuffer;
  unsigned long startTime = millis();
  char temp;
  char command[200] = "";
  int charPosition = 0;
  bool gotLine = false;
  while (millis() - startTime <= TIMEOUT) {
    while (Serial.available() > 0 && !gotLine) {
      temp = Serial.read();
      startTime = millis();
      if (temp == '\r' || temp == '\n') {
        gotLine = true;
      }
      command[charPosition++] = temp;
    }
  }
  if (gotLine) {
    JsonObject& root = jsonBuffer.parseObject(command);
    gotLine = false;
    return root;
  }
}

/*
   sprawdzenie rodzaju otrzymanej komendy i wykonanie
   odpowiedniej operacji
*/
void JsonCommands(JsonObject& jsonObject) {
  int requestId = jsonObject["id"];
  String roomName = jsonObject["name"];
  bool newState = jsonObject["state"];
  String newName = jsonObject["newRoomName"];
  int newIntensity = jsonObject["intensity"];
  bool newAutoOn = jsonObject["autoOn"];
  int newBlackout = jsonObject["blackout"];
  int dayId = jsonObject["dayId"];
  dayId = dayId - 1;
  int timeFrom = jsonObject["timeFrom"];
  int timeTo = jsonObject["timeTo"];

  if (requestId == ROOM_LIST_REQUEST) {
    sendRoomList();
    return;
  }

  if (requestId == ROOM_STATE_REQUEST) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        sendRoomState(i);
        return;
      }
    }
  }

  if (requestId == CHANGE_STATE) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].state = newState;
        return;
      }
    }
  }

  if (requestId == CHANGE_NAME) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].roomName = newName;
        return;
      }
    }
  }

  if (requestId == CHANGE_INTENSITY) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].intensity = map(newIntensity, 0 , 100 , 0 , 255);
        return;
      }
    }
  }

  if (requestId == CHANGE_AUTO_ON) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].autoOn = newAutoOn;
        return;
      }
    }
  }

  if (requestId == CHANGE_BLACKOUT) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].blackout = map(newBlackout, 0, 100, 0, 255);
        return;
      }
    }
  }

  if (requestId == OFF_ALL) {
    for (int i = 0; i < ROOM_LIST_SIZE ; i++) {
      roomList[i].state = false;
    }
  }

  if (requestId == SIMULATION_STATE) {
    sendSimulationState();
  }

  if (requestId == CHANGE_DAY_STATE) {
    //zmiana stanu dnia
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].dayList[dayId].state = newState;
        return;
      }
    }
  }

  if (requestId == CHANGE_TIME_FROM) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].dayList[dayId].timeFrom = timeFrom;
        return;
      }
    }
  }

  if (requestId == CHANGE_TIME_TO) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        roomList[i].dayList[dayId].timeTo = timeTo;
        return;
      }
    }
  }

  if (requestId == ROOM_DAYS_REQUEST) {
    for (int i = 0 ; i < ROOM_LIST_SIZE ; i++)
    {
      if (roomName == roomList[i].roomName) {
        sendDaysList(i);
        return;
      }
    }
  }

  if (requestId == CHANGE_SIMULATION_STATE) {
    simulationState = newState;
    if (newState == false){
      for(int i =0; i<ROOM_LIST_SIZE ; i++){
        roomList[i].state = false;
      }
    }
  }
}

void checkRoom() {

  for (int i = 0; i < ROOM_LIST_SIZE ; i++)
  {
    int lightSensor = map(analogRead(i), 0 , 900 , 0 , 100);
    if (roomList[i].state == true) {
      analogWrite(roomList[i].pin, roomList[i].intensity);
      continue;
    } else if (roomList[i].state == false) {
      //      analogWrite(roomList[i].pin, LOW);
      if (roomList[i].autoOn == true && (roomList[i].blackout > lightSensor)) {
        analogWrite(roomList[i].pin, roomList[i].intensity);
      } else if (roomList[i].autoOn == true &&
                 (roomList[i].blackout <= lightSensor) ||
                 (roomList[i].state == false)) {
        analogWrite(roomList[i].pin, LOW);
      }
    }
  }
}

void checkRoomInSimulation() {
  Time t = rtc.time();
  int currentTime = t.hr * 100 + t.min;
  int currentDay = t.day;
  for (int i = 0; i < ROOM_LIST_SIZE; i++) {
    for (int j = 0; j < 7; j++)
    {
      int timeF, timeT;
      timeF = roomList[i].dayList[j].timeFrom;
      timeT = roomList[i].dayList[j].timeTo;
      if ((currentTime >= timeF && currentTime <= timeT)
          && currentDay == roomList[i].dayList[j].id
          && roomList[i].dayList[j].state == true) {
        roomList[i].state = true;
        analogWrite(roomList[i].pin, roomList[i].intensity);
        break;
      } else {
        roomList[i].state = false;
        analogWrite(roomList[i].pin, LOW);
      }
    }
  }
}



/*
   glowna petla programu
*/

void loop() {
  if (Serial.available() > 0) {
    JsonObject& root = getMsg();
    JsonCommands(root);
  }
  if (simulationState == true) {
    checkRoomInSimulation();
  } else {
    checkRoom();
  }
}
