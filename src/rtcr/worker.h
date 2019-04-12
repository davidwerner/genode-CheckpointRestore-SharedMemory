/*
 * \brief  Worker thread for multi-threaded checkpointing
 * \author David Werner
 * \date   2019-03-24
 */

#ifndef _RTCR_WORKER_H_
#define _RTCR_WORKER_H_


#include <base/log.h>
#include <base/thread.h>
#include <base/component.h>
#include <base/heap.h>
#include <base/attached_rom_dataspace.h>
#include <base/printf.h>
#include <util/volatile_object.h>
#include <cpu_session/connection.h>
#include <cpu_thread/client.h>

#include "checkpointer.h"

namespace Rtcr {
	struct Job;
	class Worker;
	class Worker_pool;
	// Forward declaration
	class Target_state;
	class Checkpointer;
	class Restorer;
}

struct Rtcr::Job : Genode::List<Job>::Element
{
	unsigned type;

	Genode::size_t chunk_start;
	
	Genode::size_t chunk_size;

	Job(unsigned type, Genode::size_t chunk_start, Genode::size_t chunk_size);
};


class Rtcr::Worker : public Genode::Thread, public Genode::List<Worker>::Element
{
private:

	Genode::Env &_env;

	Rtcr::Checkpointer &_ckptr;

	Rtcr::Worker_pool &_pool;

	unsigned _id;

public:

	Worker(Genode::Env &env, const char* name, Genode::Affinity::Location location,
			Genode::Cpu_session &cpu, Rtcr::Checkpointer &ckptr,
			Rtcr::Worker_pool &pool, unsigned id);
	
	~Worker(){}

	void entry();
};


class Rtcr::Worker_pool
{
private:

	Genode::Allocator &_alloc;

	Target_state &_state;

	Rtcr::Checkpointer &_ckptr;  

	Genode::size_t _size;

	Genode::List<Job> _jobs;

	Genode::List<Worker> _workers;

public:

	Worker_pool(Genode::Allocator &alloc, Target_state &state, Rtcr::Checkpointer &ckptr, Genode::size_t size);

	~Worker_pool(){}

	void add_job(Job *job);

	Job *get_job(unsigned id);

	void start_workers();

	void join_workers();

	Genode::size_t get_size();
};

#endif /* _RTCR_WORKER_H_ */