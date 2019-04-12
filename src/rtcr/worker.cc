/*
 * \brief  Worker thread for multi-threaded checkpointing
 * \author David Werner
 * \date   2019-03-24
 */

 #include "worker.h"

 using namespace Rtcr;


 Job::Job(unsigned type, Genode::size_t chunk_start, Genode::size_t chunk_size)
 			: type(type), chunk_start(chunk_start), chunk_size(chunk_size)
 { }

 Worker::Worker(Genode::Env &env, const char* name, Genode::Affinity::Location location,
			Genode::Cpu_session &cpu, Rtcr::Checkpointer &ckptr,
			Rtcr::Worker_pool &pool, unsigned id)
 :
 	Thread(env, name, 16*1024, location, Genode::Thread::Weight(), cpu),
 	_env(env), _ckptr(ckptr), _pool(pool), _id(id)
 { }

 void Worker::entry()
 {

 	Job* job = _pool.get_job(_id);
 	
 	switch(_id)
 	{
 		case 0: { Genode::log("Worker executing: ", 0);
 				  break;
 		        }

 		case 1: { Genode::log("Worker executing: ", 1);
 				  break;
 		        }
 	}

	_ckptr._checkpoint_dataspaces_chunk(job->chunk_start, job->chunk_size); 	
 }



 Worker_pool::Worker_pool(Genode::Allocator &alloc, Target_state &state,
 					Rtcr::Checkpointer &ckptr, Genode::size_t size) 
 : 
 	_alloc(alloc), _state(state), _ckptr(ckptr), _size(size)
 {
 	Genode::Affinity::Space aff_space = _state._env.cpu().affinity_space();
	for(unsigned i=0; i<_size; i++){
		Worker* worker = new (_alloc) Worker(_state._env, "test_fred", aff_space.location_of_index((i+1)), _state._env.cpu(), _ckptr, *this, i);
		_workers.insert(worker);
	}
 }

 void Worker_pool::add_job(Job *job)
 {
 	_jobs.insert(job);
 } 

 Job *Worker_pool::get_job(unsigned id)
 {
 	Job *job = _jobs.first();

 	while(job && (id>0)){
 		job = job->next();
 		--id;
 	}

 	return job;
 }

 void Worker_pool::start_workers()
 {
 	Worker *w = _workers.first();
 	for(unsigned i=0; i<_size; i++){
		w->start();
		w = w->next();
	}
 }

 void Worker_pool::join_workers()
 { 	
 	Worker *w = _workers.first();
 	for(unsigned i=0; i<_size; i++){
		w->join();
		w = w->next();
	}
 }

 Genode::size_t Worker_pool::get_size()
 {
 	return _size;
 }


 