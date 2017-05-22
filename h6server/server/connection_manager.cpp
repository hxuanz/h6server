// ������������
#include "connection_manager.hpp"
#include "../util/my_log.h"
namespace http {
	namespace server {

		connection_manager::connection_manager()
		{
		}

		void connection_manager::start(connection_ptr c)
		{
			_INFO("�µ�����");
			_INFO("�Է���ַ�� " + c->get_address());
			connections_.insert(c);
			c->start();
		}

		void connection_manager::stop(connection_ptr c)
		{
			_INFO("�����������");
			connections_.erase(c);
			c->stop();
		}

		void connection_manager::stop_all()
		{
			_INFO("�����������");
			for (auto c : connections_)
				c->stop();
			connections_.clear();
		}

	} // namespace server
} // namespace http