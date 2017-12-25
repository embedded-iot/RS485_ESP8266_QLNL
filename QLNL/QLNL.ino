#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

ESP8266WebServer server(80);

#define RESET 4 
#define DEBUGGING
#define LED 2 


#define ADDR 0
#define ADDR_STASSID (ADDR)
#define ADDR_STAPASS (ADDR_STASSID+20)
#define ADDR_STAIP (ADDR_STAPASS+20)
#define ADDR_STAGATEWAY (ADDR_STAIP+20)
#define ADDR_STASUBNET (ADDR_STAGATEWAY+20)

#define ADDR_APSSID (ADDR_STASUBNET+20)
#define ADDR_APPASS (ADDR_APSSID+20)
#define ADDR_APIP (ADDR_APPASS+20)
#define ADDR_APGATEWAY (ADDR_APIP+20)
#define ADDR_APSUBNET (ADDR_APGATEWAY+20)

#define ADDR_PORTTCP (ADDR_APSUBNET+20)
#define ADDR_RFCONFIG (ADDR_PORTTCP+20)



#define ID_DEFAULT "12345678"

#define TIME_LIMIT_RESET 3000

#define STA_SSID_DEFAULT "G"
#define STA_PASS_DEFAULT "132654789"
#define AP_SSID_DEFAULT "MBELL.VN"
#define AP_PASS_DEFAULT ID_DEFAULT

#define USER_NAME_DEFAULT "TEST"
#define CODE_DEFAULT "1234567890"
#define URL_UPLOAD_DEFAULT "http://127.0.0.1/projects/PHP/test/updateData.php?"


bool isLogin = false;
bool isConnectAP = false;

String staSSID, staPASS;
String apSSID, apPASS;
long timeStation = 7000;
int idWebSite = 0;

bool flagClear = false;

void DEBUG(String s);
void GPIO();
void SaveStringToEEPROM(String data,int address);
String ReadStringFromEEPROM(int address);
void ClearEEPROM();
void AccessPoint();
void ConnectWifi(long timeOut);
void GiaTriThamSo();

String ContentVerifyRestart();
String ContentLogin();
String ContentConfig();
String webView();
String SendTRViewHome();
void GiaTriThamSo();


long t ;
void setup()
{
  Serial.begin(115200);
  delay(1000);
  idWebSite = 0;
  isLogin = false;
  WiFi.disconnect();
  EEPROM.begin(512);
  delay(1000);
  GPIO();
    
  if (EEPROM.read(500) != 255 || flagClear){
    ClearEEPROM();
    ConfigDefault();
    WriteConfig();
  }
  ReadConfig();
  delay(1000);

  WiFi.mode(WIFI_AP_STA);
  delay(2000);
  ConnectWifi(timeStation); 
  delay(1000);
  AccessPoint();
  delay(1000);
  StartServer();
  digitalWrite(LED,HIGH);
  delay(1000);
  // Notify: Connect AP success. 
  if (isConnectAP == false){
    int i=0;
    while (i++ < 3) {
      digitalWrite(LED,LOW);delay(500);
      digitalWrite(LED,HIGH);delay(500);
    }
  }
  show("End Setup()");
  delay(2000);
}

WiFiClient client ;
long timeLogout = 30000;
void loop()
{
  server.handleClient();
  if (millis() - t > timeLogout) {
    isLogin = false;
    t = millis();
  }
  if (digitalRead(RESET) == LOW)
  {
    //ConfigDefault();
    long t=TIME_LIMIT_RESET/100;
    while (digitalRead(RESET)==LOW && t-- >= 0){
      delay(100);
    }
    if (t < 0){
      show("RESET");
      ConfigDefault();
      WriteConfig();
      setup();
    }
  }
  delay(50);
}
void show(String s)
{
  #ifdef DEBUGGING 
    Serial.println(s);
  #endif
}
void GPIO()
{
  show("GPIO");
  pinMode(LED,OUTPUT);
  pinMode(RESET,INPUT_PULLUP);
  digitalWrite(LED,LOW);
}
void ClearEEPROM()
{
  // write a 255 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 255);
  EEPROM.commit();
  show("Clear EEPROM");
}
/*
 * Function Save String To EEPROM
 * Parameter : +data    : String Data
 *             +address : address in EEPROM
 * Return: None.
 */
void SaveStringToEEPROM(String data,int address)
{
  int len=data.length();
  EEPROM.write(address,len); 
  for (int i=1;i<=len;i++)
    EEPROM.write(address+i,data.charAt(i-1));
  EEPROM.commit();
}
/*
 * Function Read String From EEPROM
 * Parameter : +address : address in EEPROM
 * Return: String.
 */
String ReadStringFromEEPROM(int address)
{
  String s="";
  int len=(int)EEPROM.read(address);
  for (int i=1;i<=len;i++)
    s+=(char)EEPROM.read(address+i);
  return s;
}

void ConfigDefault()
{
  isLogin = false;
  staSSID = STA_SSID_DEFAULT;
  staPASS = STA_PASS_DEFAULT;
  apSSID = AP_SSID_DEFAULT;
  apPASS = AP_PASS_DEFAULT;
  //portTCP = PORT_TCP_DEFAULT;

  show("Config Default");
}
void WriteConfig()
{
  SaveStringToEEPROM(staSSID, ADDR_STASSID);
  SaveStringToEEPROM(staPASS, ADDR_STAPASS);
  SaveStringToEEPROM(apSSID, ADDR_APSSID);
  SaveStringToEEPROM(apPASS, ADDR_APPASS);
  //SaveStringToEEPROM(String(portTCP), ADDR_PORTTCP);
  
  show("Write Config");
}
void ReadConfig()
{
  staSSID = ReadStringFromEEPROM(ADDR_STASSID);
  staPASS = ReadStringFromEEPROM(ADDR_STAPASS);
  apSSID = ReadStringFromEEPROM(ADDR_APSSID);
  apPASS = ReadStringFromEEPROM(ADDR_APPASS);
  //portTCP = atol(ReadStringFromEEPROM(ADDR_PORTTCP).c_str());
  show("Read Config");
}

void AccessPoint()
{
  show("Access Point Config");
  //WiFi.disconnect();
  delay(1000);
  // Wait for connection
  show( WiFi.softAP(apSSID.c_str(),apPASS.c_str()) ? "Ready" : "Failed!");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  String SoftIP = ""+(String)myIP[0] + "." + (String)myIP[1] + "." +(String)myIP[2] + "." +(String)myIP[3];
  show(SoftIP);
  
}

void ConnectWifi(long timeOut)
{
  show("Connect to other Access Point");
  delay(1000);
  int count = timeOut / 200;
  show("Connecting");
  show(staSSID);
  show(staPASS);
  WiFi.begin(staSSID.c_str(),staPASS.c_str());
  
  while (WiFi.status() != WL_CONNECTED && --count > 0) {
    delay(200);
    Serial.print(".");
  }
  if (count > 0){
    show("Connected");
    IPAddress myIP = WiFi.localIP();
    String LocalIP = ""+(String)myIP[0] + "." + (String)myIP[1] + "." +(String)myIP[2] + "." +(String)myIP[3];
    show("Local IP :"); 
    show(LocalIP);
    isConnectAP = true;
  }else {
    isConnectAP = false;
    show("Disconnect");
  }
}

void StartServer()
{
  server.on("/", webConfig);
  server.on("/home", webViewHome);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void webConfig() {
  GiaTriThamSo();
  String html = Title();
  if (idWebSite == 0) {
    html += ContentLogin();
  }
  else if (idWebSite == 1) {
    html += ContentConfig();
  }
  else if (idWebSite == 2) {
    html += ContentVerifyRestart();
  }else html += ContentLogin();
  server.send ( 200, "text/html",html);
}

void webViewHome() {
  String html = Title();
  html += webView();
  server.send ( 200, "text/html",html);
}


String Title(){
  String html = "<html>\
  <head>\
  <meta charset=\"utf-8\">\
  <title>Config</title>\
  <style>\
    * {margin:0;padding:0}\
    body {width: 600px;height: auto;border: red 3px solid; margin: 0 auto; box-sizing: border-box}\
    .head1{ display: flex; height: 50px;border-bottom: red 3px solid;}\
    .head1 h1{margin: auto;}\
    table, th, td { border: 1px solid black;border-collapse: collapse;}\
    tr{ height: 40px;text-align: center;font-size: 20px;}\
    input { height: 25px;text-align: center;}\
    button {height: 25px;width: 100px;margin: 5px;}\
    button:hover {background: #ccc; font-weight: bold; cursor: pointer;}\
    .subtitle {text-align: left;font-weight: bold;}\
    .content {padding: 10px 20px;}\
    .left , .right { width: 50%; float: left;text-align: left;line-height: 25px;padding: 5px 0;}\
    .left {text-align: right}\
    .listBtn {text-align: center}\
    a {text-decoration: none;}\
    table {width: 100%;}\
    .column {width: 50%;text-align: center;}\
    .noboder {border: none;}\
  </style>\
  </head>";
  return html;
}
String ContentVerifyRestart() {
  String content = "<body>\
    <div class=\"head1\">\
      <h1>Restart Device</h1>\
    </div>\
    <div class=\"content\">\
      <form action=\"\" method=\"get\">\
      <div class=\"subtitle\">Do you want restart device?</div>\
      <div class=\"listBtn\">\
        <button type=\"submit\" name=\"txtVerifyRestart\" value=\"false\">No</button>\
        <button type=\"submit\" name=\"txtVerifyRestart\" value=\"true\">Yes</button>\
      <div>\
      </form>\
    </div>\
  </body>\
  </html>";
  return content;
}
String ContentLogin(){
  String content = "<body>\
    <div class=\"head1\">\
      <h1>Login Config</h1>\
    </div>\
    <div class=\"content\">\
      <form action=\"\" method=\"get\">\
        <div class=\"left\">Name Access Point </div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Tên wifi\" name=\"txtNameAP\" required></div>\
        <div class=\"left\">Password Port TCP</div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Cổng TCP\" name=\"txtPassPortTCP\" required></div>\
        <div class=\"listBtn\">\
      <button type=\"submit\">Login</button></div>\
      </form>\
    </div>\
  </body>\
  </html>";
  return content;
}
String ContentConfig(){
  String content = "<body>\
    <div class=\"head1\">\
      <h1>Setting config</h1>\
    </div>\
    <div class=\"content\">\
      <form action=\"\" method=\"get\">\
        <div class=\"subtitle\">Station mode (Connect to other Access Point)</div>\
        <div class=\"left\">Name Access Point </div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Tên wifi\" name=\"txtSTAName\" value=\""+staSSID+"\" required></div>\
        <div class=\"left\">Password Access Point</div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Mật khấu wifi\" name=\"txtSTAPass\" value=\""+staPASS+"\"></div>\
        <div class=\"left\">Status</div>\
        <div class=\"right\">: "+(isConnectAP == true ? "Connected" : "Disconnect")+"</div>\
        <div class=\"subtitle\">Access Point mode (This is a Access Point)</div>\
        <div class=\"left\">Name</div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Tên wifi phát ra\" name=\"txtAPName\" value=\""+apSSID+"\" required></div>\
        <div class=\"left\">Password</div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Mật khẩu wifi phát ra\" name=\"txtAPPass\" value=\""+apPASS+"\"></div>\
        <div class=\"subtitle\">Server Upload Data</div>\
        <div class=\"left\">User Name</div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Tên tài khoản\" name=\"txtUseName\" value=\""+USER_NAME_DEFAULT+"\" required></div>\
        <div class=\"left\">Code</div>\
        <div class=\"right\">: <input class=\"input\" placeholder=\"Mã thiết bị\" name=\"txtcode\" value=\""+CODE_DEFAULT+"\" required></div>\
        <div class=\"left\">URL Upload Data</div>\
        <div class=\"right\">: <input class=\"input\" type=\"url\" placeholder=\"Mã thiết bị\" name=\"txtcode\" value=\""+URL_UPLOAD_DEFAULT+"\" required></div>\
        <hr>\
        <div class=\"listBtn\">\
          <button type=\"submit\"><a href=\"?txtRefresh=true\">Refresh</a></button>\
          <button type=\"submit\" name=\"btnSave\" value=\"true\">Save</button>\
          <button type=\"submit\"><a href=\"?txtRestart=true\">Restart</a></button>\
          <button type=\"submit\"><a href=\"?txtLogout=true\">Logout</a></button>\
        </div>\
      </form>\
    </div>\
  </body>\
  </html>";
  return content;
}

String webView(){
  String content = "<body>\
    <div class=\"head1\">\
    <h1>HOME PAGE</h1>\
    </div>\
    <div class=\"content\">\
    <form action=\"\" method=\"get\">\
      <table>\
      <tr class=\"row\"><th>ID</th><th>NAME RF</th></tr>"+ SendTRViewHome() +"\
      </table>\
      <br><hr>\
      <div class=\"listBtn\">\
      <button type=\"submit\"><a href=\"/login\">Login</a></button>\
      </div>\
    </form>\
    <script type=\"text/javascript\">\
      setInterval(function() {\
      window.location.reload();\
      }, 2000);\
    </script>\
    </div>\
  </body>\
  </html>";
  return content;
}
String SendTRViewHome()
{
  String s="";
  //s += "<tr class=\"row" + isTrActive(i) + "\"><td class=\"column\">"+ id + "-" + getAddress(bufferRF[i]) +"</td><td class=\"column\">"+ getData(bufferRF[i]) +"</td></tr>";
  return s;
}

void GiaTriThamSo()
{
  t = millis();
  String message="";
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  String UserName, PassWord;
  for (uint8_t i=0; i<server.args(); i++){
     
    String Name=server.argName(i); 
    String Value=String( server.arg(i)) ;
    String s1=Name+ ": " +Value;
    //show(s1);
    if (isLogin == true) {
      if (Name.indexOf("txtLogout") >= 0){
        isLogin = false;
        show("Logout");
      }
      else if (Name.indexOf("txtSTAName") >= 0){
        if (Value != staSSID && Value.length() > 0){
          staSSID =  Value ;
          show("Set staSSID : " + staSSID);
        }
      }
      else if (Name.indexOf("txtSTAPass") >= 0){
        if (Value != staPASS) {
          staPASS =  Value ;
          show("Set staPASS : " + staPASS);
        }
      }
      else if (Name.indexOf("txtAPName") >= 0){
        if (Value != apSSID && Value.length() > 0){
          apSSID =  Value ;
          show("Set apSSID : " + apSSID);
        }
      }
      else if (Name.indexOf("txtAPPass") >= 0){
        if (Value != apPASS){
          apPASS =  Value ;
          show("Set apPASS : " + apPASS);
        }
      }
      else if (Name.indexOf("txtRestart") >= 0){
        idWebSite = 2;
        show("Verify restart");
        show(Value);
      }
      else if (Name.indexOf("btnSave") >= 0)
      {
        WriteConfig();
        show("Save config");
      }
      else if (Name.indexOf("txtVerifyRestart") >= 0)
      {
        if ( Value.indexOf("true") >=0 ) {
          setup();
          show("Restart Device");
          idWebSite = 0;
        }
        else idWebSite = 1;
      }
    }else {
      if (Name.indexOf("txtNameAP") >= 0)
        UserName =  Value ;
      else if (Name.indexOf("txtPassPortTCP") >= 0)
        PassWord =  Value ;

      if (UserName.equals(apSSID) && PassWord.equals(apPASS)){
        isLogin = true;
        idWebSite = 1;
        show("Login == true");
      }else {
        idWebSite = 0;
        isLogin = false;
      }
    }
    Name = "";
    Value = "";
  }
  
  if (isLogin == false)
    idWebSite = 0;
}
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



