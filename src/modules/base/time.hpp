// Author: Dmitry Kukovinets (d1021976@gmail.com)


// Return current time in milliseconds
inline
std::time_t
base::utc_time() noexcept
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
