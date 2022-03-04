// Artigo de referencia para upload de codigo via OTA https://medium.com/google-cloud/serverless-continuous-integration-and-ota-update-flow-using-google-cloud-build-and-arduino-d5e1cda504bf

#include "main.h"

WiFiServer server(WIFI_SERVER_PORT);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void toggleOutputA1(boolean state)
{
  digitalWrite(RELE_N4, !state);
  digitalWrite(LED_BLUE, !state);
}

void setup()
{
  Serial.begin(MONITOR_SPEED);
  WiFiManager wifiManager;

  pinMode(LED_BLUE, OUTPUT);
  pinMode(RELE_N4, OUTPUT);

  wifiConnect();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  config.signer.test_mode = FIREBASE_TEST_MODE;
}

void loop()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    wifiConnect();
  }

  if (Firebase.ready() && (millis() - dataMillis > 2000 || dataMillis == 0))
  {
    dataMillis = millis();

    String documentPath = "irrigation/Harbiq84b3LeyVzPL8QE/";
    String mask = "state";

    Serial.print("Get a document... ");

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str()))
    {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());

      // Create a FirebaseJson object and set content with received payload
      FirebaseJson payload;
      payload.setJsonData(fbdo.payload().c_str());

      // Get the data from FirebaseJson object
      FirebaseJsonData jsonData;
      payload.get(jsonData, "fields/state/booleanValue", true);
      Serial.println(jsonData.stringValue);

      toggleOutputA1(jsonData.boolValue);
    }
    else
      Serial.println(fbdo.errorReason());
  }
}