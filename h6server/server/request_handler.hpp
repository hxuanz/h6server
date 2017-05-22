#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>
#include "../master.h"

namespace http {
	namespace server {

		struct reply;
		struct request;

		/// The common handler for all incoming requests.
		class request_handler
		{
		public:
			request_handler(const request_handler&) = delete;  // �������캯��
			request_handler& operator=(const request_handler&) = delete;

			/// Construct with a directory containing files to be served.
			explicit request_handler(Master& mater);

			/// Handle a request and produce a reply.
			void handle_request(const request& req, reply& rep);

		private:
			/// The directory containing the files to be served.
			Master& mater_;
		};

	} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP