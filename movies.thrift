namespace cpp movies
namespace py movies
namespace lua movies


enum ErrorCode {
  SE_CONNPOOL_TIMEOUT,
  SE_THRIFT_CONN_ERROR,
  SE_UNAUTHORIZED,
  SE_MEMCACHED_ERROR,
  SE_MONGODB_ERROR,
  SE_REDIS_ERROR,
  SE_THRIFT_HANDLER_ERROR,
  SE_RABBITMQ_CONN_ERROR
}

exception ServiceException {
    1: ErrorCode errorCode;
    2: string message;
}

service RecommenderService{
  list<string> GetRecommendations(
    1: i64 user
  )throws (1: ServiceException se)
}

service MovieInfoService{
  list<string> GetMoviesByIds(
    1: list<string> movie_ids
  )
}

service UserLikesService{
	list<i64> GetMovieLikesByIds(
     1: list<string> movie_ids
   )
   void LikeDislikeMovie(
     1: string user_id
     2: string movie_id
     3: bool likeDislike
   )
}
