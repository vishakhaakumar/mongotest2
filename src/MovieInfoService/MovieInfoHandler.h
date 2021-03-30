#ifndef MOVIES_MICROSERVICES_MOVIEINFOHANDLER_H
#define MOVIES_MICROSERVICES_MOVIEINFOHANDLER_H

#include <iostream>
#include <string>
#include <regex>
#include <future>

#include "../../gen-cpp/MovieInfoService.h"

#include "../ClientPool.h"
#include "../ThriftClient.h"
#include "../logger.h"

namespace movies{

class MovieInfoServiceHandler : public MovieInfoServiceIf {
 public:
  MovieInfoServiceHandler(ClientPool<ThriftClient<MovieInfoServiceClient>> *);
  ~MovieInfoServiceHandler() override=default;

  void GetMoviesByIds(std::vector<std::string>& _return, const std::vector<std::string> & movie_ids) override;
  private:
  ClientPool<ThriftClient<MovieInfoServiceClient>> *_movie_mongodb_client_pool;
};

// Constructor
MovieInfoServiceHandler::MovieInfoServiceHandler(
ClientPool<ThriftClient<MovieInfoServiceClient>> *movie_mongodb_client_pool) {
  _movie_mongodb_client_pool = movie_mongodb_client_pool;
}

// Remote Procedure "GetMoviesById"

void MovieInfoServiceHandler::GetMoviesByIds(std::vector<std::string>& _return, const std::vector<std::string> & movie_ids) {
   printf("GetMoviesByIds called \n");
  std::cout << "GetMoviesByIds here ...!!!!!!!!" << std::endl;
 
 // testing  Get the movie info service client pool
    auto movie_info_client_wrapper = _movie_mongodb_client_pool->Pop();
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
 
   _return.push_back("Muppets Take Manhattan I");
   _return.push_back("The Lion Kingi II");
}

} // namespace movies


#endif //MOVIES_MICROSERVICES_MOVIEINFOHANDLER_H

