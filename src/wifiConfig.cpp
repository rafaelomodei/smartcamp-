#include "wifiConfig.h"

bool SHOWLD_SAVE_CONFIG = false;

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
  SHOWLD_SAVE_CONFIG = true;
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
  const char AP_NAME[sizeof(WIFI_NAME)] = WIFI_NAME;

  WiFiManager wifiManager;

  if (!wifiAutoConnect(&wifiManager, AP_NAME))
  {
    wifiStartConfigPortal(&wifiManager, AP_NAME);
  }
}
