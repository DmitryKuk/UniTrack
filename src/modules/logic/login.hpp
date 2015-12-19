// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Starts new session for user using data from login form.
// Returns user ref and session cookie, if user logged in successfully.
// Otherwise, throws.
inline
std::pair<std::string, std::string>
logic::login::login_user(const logic::login::form &form) const
{
	using namespace std::literals;
	return this->start_session_for_email(form.at("email"s), form.at("password"s));
}
