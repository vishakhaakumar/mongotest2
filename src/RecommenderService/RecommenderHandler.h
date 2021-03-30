#ifndef MOVIES_MICROSERVICES_RECOMMENDERHANDLER_H
#define MOVIES_MICROSERVICES_RECOMMENDERHANDLER_H

#include <iostream>
#include <string>
#include <regex>
#include <future>

#include "../../gen-cpp/RecommenderService.h"
#include "../../gen-cpp/MovieInfoService.h"
#include "../../gen-cpp/UserLikesService.h"

#include "../ClientPool.h"
#include "../ThriftClient.h"
#include "../logger.h"

namespace movies{

class RecommenderServiceHandler : public RecommenderServiceIf {
 public:
  RecommenderServiceHandler(
		  ClientPool<ThriftClient<MovieInfoServiceClient>> *,
		  ClientPool<ThriftClient<UserLikesServiceClient>> *) ;
  ~RecommenderServiceHandler() override=default;

  void GetRecommendations(std::vector<std::string>& _return, const int64_t user) override;
 private:
  ClientPool<ThriftClient<MovieInfoServiceClient>> *_movie_info_client_pool;
  ClientPool<ThriftClient<UserLikesServiceClient>> *_user_likes_client_pool;
};

// Constructor
RecommenderServiceHandler::RecommenderServiceHandler(
		ClientPool<ThriftClient<MovieInfoServiceClient>> *movie_info_client_pool,
		ClientPool<ThriftClient<UserLikesServiceClient>> *user_likes_client_pool) {

     // Storing the clientpool
     _movie_info_client_pool = movie_info_client_pool;
	 _user_likes_client_pool = user_likes_client_pool;
}

// Remote Procedure "GetRecommendations"
void RecommenderServiceHandler::GetRecommendations(std::vector<std::string>& _return, const int64_t user){
    
    // Get recommended movie ids for this user
    std::vector<std::string> movie_ids;
    movie_ids.push_back("abcd");
    movie_ids.push_back("efgh");

    // Get the movie info service client pool
    auto movie_info_client_wrapper = _movie_info_client_pool->Pop();
    if (!movie_info_client_wrapper) {
      ServiceException se;
      se.errorCode = ErrorCode::SE_THRIFT_CONN_ERROR;
      se.message = "Failed to connect to movie-info-service";
      throw se;
    }
    auto movie_info_client = movie_info_client_wrapper->GetClient();

    // Call the remote procedure : GetMoviesByIds
    std::vector<std::string> _return_movies;
    try {
      movie_info_client->GetMoviesByIds(_return_movies, movie_ids);
      _return = _return_movies;
    } catch (...) {
      _movie_info_client_pool->Push(movie_info_client_wrapper);
      LOG(error) << "Failed to send call GetMoviesByIds to movie-info-client";
      throw;
    }
    _movie_info_client_pool->Push(movie_info_client_wrapper);
	
	// Get the user likes service client pool
    auto user_likes_client_wrapper = _user_likes_client_pool->Pop();
    if (!user_likes_client_wrapper) {
      ServiceException se;
      se.errorCode = ErrorCode::SE_THRIFT_CONN_ERROR;
      se.message = "Failed to connect to user-likes-service";
      throw se;
    }
    auto user_likes_client = user_likes_client_wrapper->GetClient();

    // Call the remote procedure : GetMovieLikesByIds
    std::vector<int64_t> _return_movie_likes;
    try {
		// Doesn't currently change output, call is simply made to check if error occurs
      user_likes_client->GetMovieLikesByIds(_return_movie_likes, movie_ids);
    } catch (...) {
      _user_likes_client_pool->Push(user_likes_client_wrapper);
      LOG(error) << "Failed to send call GetMovieLikesByIds to user-likes-client";
      throw;
    }
    _user_likes_client_pool->Push(user_likes_client_wrapper);
	
	// Call the remote procedure : LikeDislikeMovie
	std::string user_id = "123";
	std::string user_movie_id = "xyz";
	bool user_like_dislike = false;
    try {
		// Doesn't currently do anything, call is simply made to check if error occurs
      user_likes_client->LikeDislikeMovie(user_id, user_movie_id, user_like_dislike);
    } catch (...) {
      _user_likes_client_pool->Push(user_likes_client_wrapper);
      LOG(error) << "Failed to send call LikeDislikeMovie to user-likes-client";
      throw;
    }
    _user_likes_client_pool->Push(user_likes_client_wrapper);
}

} // namespace movies


#endif //MOVIES_MICROSERVICES_RECOMMENDERHANDLER_H

