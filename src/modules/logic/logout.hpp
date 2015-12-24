// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Finishes user session.
// If session is invalid, throws.
inline
void
logic::logout::logout_user(const std::string &session_id) const
{
	return this->finish_session(session_id);
}
