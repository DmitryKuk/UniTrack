// Author: Dmitry Kukovinets (d1021976@gmail.com)


inline
logic::enable_global_instance_ref::enable_global_instance_ref(global_instance &logic_gi) noexcept:
	logic_gi_{logic_gi}
{}


inline
logic::global_instance &
logic::enable_global_instance_ref::logic_gi() noexcept
{
	return this->logic_gi_;
}
