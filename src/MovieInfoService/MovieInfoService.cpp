#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <signal.h>

#include "../utils.h"
#include "MovieInfoHandler.h"

using json = nlohmann::json;
using apache::thrift::server::TThreadedServer;
using apache::thrift::transport::TServerSocket;
using apache::thrift::transport::TFramedTransportFactory;
using apache::thrift::protocol::TBinaryProtocolFactory;

using namespace movies;


// signal handler code
void sigintHandler(int sig) {
	exit(EXIT_SUCCESS);
}

// entry of this service
int main(int argc, char **argv) {
  // 1: notify the singal handler if interrupted
  signal(SIGINT, sigintHandler);
  // 1.1: Initialize logging
  init_logger();


  // 2: read the config file for ports and addresses
  json config_json;
  if (load_config_file("config/service-config.json", &config_json) != 0) {
    exit(EXIT_FAILURE);
  }

  // 3: get my port
  int my_port = config_json["movie-info-service"]["port"];
	
  // 4a: get the movie info service's port and address
  int movie_info_mongodb_port = config_json["movie-info-mongodb"]["port"];
  std::string movie_info_mongodb_addr = config_json["movie-info-mongodb"]["addr"];
	
   std::cout << "Mongodb port and addr done ..." << std::endl;
 
  // 4b: get the client of movie-info-service
  ClientPool<ThriftClient<MovieInfoServiceClient>> movie_mongodb_client_pool(
      "movie-info-service", movie_info_mongodb_addr, movie_info_mongodb_port, 0, 128, 1000);
	
	std::cout << "Mongodb client done ..." << std::endl;
	
	// testing  Get the movie info service client pool
    auto movie_info_client_wrapper = movie_mongodb_client_pool->Pop();
    if (!movie_info_client_wrapper) {
	std::cout << "ERROR here ..." << std::endl;
      ServiceException se;
      se.errorCode = ErrorCode::SE_THRIFT_CONN_ERROR;
      se.message = "Failed to connect to movie-info-service";
      throw se;
    }
	std::cout << "Client pool pop done !!! ..." << std::endl;
    auto movie_info_client = movie_info_client_wrapper->GetClient();
	std::cout << "GetClient done !!! ..." << std::endl;

  // 5: configure this server
  TThreadedServer server(
      std::make_shared<MovieInfoServiceProcessor>(
          std::make_shared<MovieInfoServiceHandler>()),
      std::make_shared<TServerSocket>("0.0.0.0", my_port),
      std::make_shared<TFramedTransportFactory>(),
      std::make_shared<TBinaryProtocolFactory>()
  );
  
  // 5: start the server
  std::cout << "Starting the movie info server ..." << std::endl;
  server.serve();
  return 0;
}

