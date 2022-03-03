#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

#define API_KEY "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC3a9vgxkSeudrm\npvWrilupKhehiYyWE7d5jtnDefcHGTCZyLANI5FvMblYFkikqEhKfsu8fEQ2EhRd\nFvFkgdOQspe+EZM+RW32usingnw4eeBtV7VzRIoo1eyTtbBiLXLPM3NBMDvwhvt3\n8+Z6fgcYyBalj7svtoRKQMGSonc5Hr3vZacr+9HhB976JbNTNAkSLFpEL30w2bYI\n4IjBPcxzhGLIjmGfcymFjZZE9sztSWz5erMWTgpscYU9peZba3lkkZjIGgaRYBs2\nM/bLQKUEiJ6/394LJmLOiz7+Hby5VsSLTvSbeWFCM3Nbtem8LmtNlCDBU2PdSdc6\n7cNCtX+fAgMBAAECggEADSw526gMQDn+IUKBsj163RNLedZ0ZPLVjRgCm6n5YREg\nub/9T3RBJZ3fPxbv8KNaoxRSyB6Z532KPi/qS4WSDs+aBj8iNqlV1GUQXrddpHtc\nRX4JvKrXdpymeOqQ2FGF7/Y0o3c2AXgwPH/PtaGyG3LYmDCns4LazVpejIwOc+jv\ndEV9GQjf9a2kitd3qFyS2K24/CRFpni6cuuoUjEACIUtpGyvdEAulLie+QC9DhFD\n7Oryq3ieBpqql52vi03nuXh3TViCSG7ECm+5k2gqzKPoh65giM5P7snGy2Ga7zct\n6kml80nFQVnvBOX7/s2GlHnOHozI78tPnSX3+x0MQQKBgQDlAr+nZzfgtdpxu5WI\n7MGGBVqNSh8T9ES2xmXceRkIzfew4kvHMRwinZWT/hBfFUEVkqfWhYedks5I8X6m\nViJwxg+UXQWUDFyilcfBMEETYBB7zKrGpuP8H6gcXX++h3qMUSlJCs6wzuVsulDG\nU9/9hLcdrXYvNyObUFrNPweoGwKBgQDNCa2co85Uvxss9tbPd8RudV09t+3LItpH\nAJZ9Z7DmOqju4AqyKsUa8gUm4mbAlUJlImzGK4Bu7E97HyzOsEi5UJwVN5md1gkH\nkrqarL5luSK3Ih2VMBtYMQrdBD1XWewKUFzF/ANs+xvSMEelZS2AYuCkWHTNkJjZ\njXF3n53GzQKBgAiM/Lj0aqqyP5DFj0TIikVFx+bEoLpvAW3NpJoBz4R7pEbvfCLY\n25MTTTV0ZAoY0hBLJIcm/zRL+fXRRohRaQHK82P3wTCQ8ZiEfSvicFuX16W3s5yt\nOI/+YpmEiDOF/XZlxWPbynHhl8Fih4eQNCnUHb427tW+ek7fRjSpqNCHAoGAE0tA\n2geUB+d/0CheKGG1y0sh/gfLyZy6+4lzn9NdEx0b/nZ4qzFZIOL7VyBg2Ohyuf+W\nSsOXL1jvVb+mQMDX5xBNz7VOsEqOyPamtp1gwUO9n4wHSp2fRRpAUnHur8SXcJUF\nVeAUFnI+uPWwGEFAGd+0EeL9AscDix1DOyV6/FUCgYEA3WiUzIBH6GDXMAGthQgk\npAYYG3pRaDQliIoyEKoutPI8GhyQ1Vwlx2DoWYEJSWb8gNNv5KpiwbKDtNhHZKko\nVOCP13aqFONPacaPLTPBGiVE3rCvP7iAov3J8XMXyLf4eJYL3miHlBE1Z5uKlO1v\nH2g65hocIkk/dM13xigi/Vk="
#define FIREBASE_PROJECT_ID "smartcamp-9c56a"
#define USER_EMAIL "omodei@outlook.com"
#define USER_PASSWORD "12345678"

#define RELE_N4 16
#define LED_BLUE 15
#define LED_GREEN 13
#define LED_RED 2

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long dataMillis = 0;
int count = 0;
bool taskCompleted = false;

WiFiServer server(3001);
const int PIN_LED = D4;

String request = "";

unsigned long time_now = 0;
unsigned long time_prev = 0;

bool shouldSaveConfig = false;

// The Firestore payload upload callback function
void fcsUploadCallback(CFS_UploadStatusInfo info)
{
  if (info.status == fb_esp_cfs_upload_status_init)
  {
    Serial.printf("\nUploading data (%d)...\n", info.size);
  }
  else if (info.status == fb_esp_cfs_upload_status_upload)
  {
    Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_cfs_upload_status_complete)
  {
    Serial.println("Upload completed ");
  }
  else if (info.status == fb_esp_cfs_upload_status_process_response)
  {
    Serial.print("Processing the response... ");
  }
  else if (info.status == fb_esp_cfs_upload_status_error)
  {
    Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
  }
}

// Coenectando com o wifi informado
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

// callback nos notificando sobre a necessidade de salvar a configuração
void saveConfigCallback()
{
  Serial.println("Salvando as configurações da rede WIFI");
  shouldSaveConfig = true;
}

// Ao iniciar, vai tentar conectar no wifi já cadastrado
int wifiAutoConnect(WiFiManager *wifiManager, const char *AP_NAME)
{

  bool state = false;

  if (WiFi.status() != WL_CONNECTED)
  {
    // wifiManager->resetSettings();

    wifiManager->setAPCallback(configModeCallback);
    wifiManager->setSaveConfigCallback(saveConfigCallback);
    wifiManager->autoConnect(AP_NAME);
  }

  WiFi.status() != WL_CONNECTED ? state = 0 : state = 1;

  return state;
}

// Caaso a internet caia, ele tenta reconectar
void wifiStartConfigPortal(WiFiManager *wifiManager, const char *AP_NAME)
{

  if (wifiManager->startConfigPortal(AP_NAME))
  {
    Serial.println("Falha ao conectar");
    delay(2000);
    ESP.restart();
  }
}

// Configuração da rede wifi
void wifiConnect()
{
  const char AP_NAME[10] = "SMARTCAMP";

  WiFiManager wifiManager;

  if (!wifiAutoConnect(&wifiManager, AP_NAME))
  {
    wifiStartConfigPortal(&wifiManager, AP_NAME);
  }
}

void toggleOutputA1(boolean state){
  digitalWrite(RELE_N4, !state);
  digitalWrite(LED_BLUE, !state);
}

void setup()
{
  Serial.begin(9600);
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

  config.signer.test_mode = true; //TODO: remover o modo de teste
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
    String mask ="state";

    Serial.print("Get a document... ");

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str())){
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