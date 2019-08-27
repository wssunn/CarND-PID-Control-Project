#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  /**
   * TODO: Initialize the pid variable.
   */
  double k_p = 0.07;
  double k_i = 0.0001;
  double k_d = 0.8;
  pid.Init(k_p, k_i, k_d);

  PID pid_throttle;
  pid_throttle.Init(0.1, 0.00001, 0.2);

  double cte_prev = 0.0;
  double cte_delta = 0.0;

  h.onMessage([&pid, &pid_throttle, &cte_delta, &cte_prev](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          double throttle_value = 0.30;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          pid.UpdateError(cte);
          steer_value = pid.TotalError();
          if (abs(steer_value) > 0.17){
            //set a limit to steering angle
            steer_value = abs(steer_value)/steer_value * 0.17;
          }

          pid_throttle.UpdateError(cte * cte - 0.25);
          //normal driving, exluding bend
          if (speed > 5.0 && speed < 20.0 && abs(cte) < 1.5 && abs(cte_delta) < 0.3)
          {
            throttle_value += pid_throttle.TotalError();
          }
          //when at a bend
          else if (abs(cte_delta) > 0.3 || abs(cte) > 1.5){
            //first reduce speed
            if (speed > 10.0) {throttle_value = -0.35;}
            //then slowly move out
            else {throttle_value = 0.25;}
            steer_value *= 3.0; //quickly fix
          }
          //speed limit
          else if (speed > 20.0) { throttle_value = 0.01;}
          //low speed throttle setting
          else {
            throttle_value = 0.15;
            if (speed < 3.0){throttle_value *= 4;}
            else if (speed < 5.0){throttle_value *= 2;}
          }

          cte_delta = cte - cte_prev;
          cte_prev = cte;
          
          
          // DEBUG
          // std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
          //           << std::endl;
          // std::cout << "throttle" << throttle_value << std::endl;
          // std::cout << "speed:" << speed << "angle: " << angle << std::endl;
          std::cout << "CTE: " << cte << std::endl;
          std::cout << "change of CTE" << cte_delta << std::endl;


          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle_value;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}