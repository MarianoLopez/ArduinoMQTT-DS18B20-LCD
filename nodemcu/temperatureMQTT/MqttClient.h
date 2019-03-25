#include <PubSubClient.h>
class MqttClient{
  private:
    const char* mqtt_client;
    const char* mqtt_user;
    const char* mqtt_password;
    IPAddress mqtt_server;
    int mqtt_port;
    PubSubClient client;
  public:
    MqttClient(){}
    MqttClient(Client& _client,IPAddress server,const int port,const char* clientName,const char* user,const char* password){
      client = _client;
      mqtt_client = clientName;
      mqtt_user = user;
      mqtt_password = password;
      mqtt_server = server;
      mqtt_port = port;
      client.setServer(server, port);//set mqtt server
    }
    /*if mqtt client is not connected will try to connect with @mqtt_client, @mqtt_user & @mqtt_password*/
    String connect(){
      if (!client.connected() || client.state()!=MQTT_CONNECTED) {
        toLogln("Connecting MQTT: "+String(mqtt_user)+"@"+IPAddress2String(mqtt_server)+":"+mqtt_port+"  client: "+String(mqtt_client));
        client.connect(mqtt_client,mqtt_user,mqtt_password);
      }
      String response =  getStatus();
      toLogln(response);
      return response;
    }

  /*send @message to @topic if mqtt client is connected else will try to connect once*/
  boolean send(const char* message,const char* topic){
    if(!client.connected() || client.state()!=MQTT_CONNECTED){
      connect();
    }
    return client.publish(topic, message); 
  }

  /*@return String mqtt connection status enum*/
  String getStatus(){
    switch(client.state()){
        case MQTT_CONNECTION_TIMEOUT:
          return "MQTT_CONNECTION_TIMEOUT: the server didn't respond within the keepalive time";
        case MQTT_CONNECTION_LOST:
          return "MQTT_CONNECTION_LOST: the network connection was broken";
        case MQTT_CONNECT_FAILED:
          return "MQTT_CONNECT_FAILED: the network connection failed";
        case MQTT_DISCONNECTED:
          return "MQTT_DISCONNECTED: the client is disconnected cleanly";
        case MQTT_CONNECTED:
          return "MQTT_CONNECTED: the client is connected";
        case MQTT_CONNECT_BAD_PROTOCOL:
          return "MQTT_CONNECT_BAD_PROTOCOL: the server doesn't support the requested version of MQTT";
        case MQTT_CONNECT_BAD_CLIENT_ID:
          return "MQTT_CONNECT_BAD_CLIENT_ID: the server rejected the client identifier";
        case MQTT_CONNECT_UNAVAILABLE:
          return "MQTT_CONNECT_UNAVAILABLE: the server was unable to accept the connection";
        case MQTT_CONNECT_BAD_CREDENTIALS:
          return "MQTT_CONNECT_BAD_CREDENTIALS: the username/password were rejected";
        case MQTT_CONNECT_UNAUTHORIZED:
          return "MQTT_CONNECT_UNAUTHORIZED: the client was not authorized to connect";
        default:
          return "unknow error";
      }
  }
};
