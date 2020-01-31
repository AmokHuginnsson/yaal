/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hworkflow.hxx
 * \brief Declaration of HWorkFlow class.
 */

#ifndef YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED
#define YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hlist.hxx"

namespace yaal {

namespace tools {

/*! \brief Thread Pool idiom implementation.
 */
class HWorkFlow {
public:
	typedef HWorkFlow this_type;
	typedef yaal::hcore::HPointer<HWorkFlow> ptr_t;
	/*! \brief Callable type to represent task.
	 *
	 * Also type to represent means for asynchronous notification that task should be stopped.
	 */
	typedef yaal::hcore::HBoundCall<> call_t;
	typedef yaal::hcore::HBoundCall<bool()> want_restart_t; /*!< Callable type to represent means for inquiring task if it should be restarted. */

	/*! \brief Current state of HWorkFlow.
	 */
	enum class STATE {
		RUNNING,      /*!< This HWorkFlow is working normally. */
		STOPPING,     /*!< This HWorkFlow is currently being stopped but new tasks can be scheduled. */
		INTERRUPTING, /*!< This HWorkFlow is currently being stopped but new tasks can be scheduled. */
		STOPPED,      /*!< This HWorkFlow is stopped now, no tasks are running but new tasks can be scheduled. */
		ABORTING,     /*!< This HWorkFlow is being aborted right now, new tasks are rejected. */
		CLOSING,      /*!< This HWorkFlow is currently being stopped but new tasks are rejected. */
		CLOSED        /*!< This HWorkFlow is stopped now, new tasks are rejected. */
	};

	/*! \brief Mode for winding up HWorkFlow.
	 */
	enum class WINDUP_MODE {
		ABORT,     /*!< Try to interrupt currently running tasks, drop (abort) scheduled tasks, new tasks are rejected. _state -> ABORTING */
		INTERRUPT, /*!< Interrupt currently running tasks, keep scheduled tasks, new tasks can be scheduled. _state -> INTERRUPTING */
		SUSPEND,   /*!< Finish currently running tasks normally, new tasks can be scheduled. _state -> STOPPING */
		CLOSE      /*!< Finish currently running tasks normally, new tasks are rejected. _state -> STOPPING */
	};

	/*! \brief Schedule policy for new tasks.
	 */
	enum class SCHEDULE_POLICY {
		EAGER, /*!< Start execution of a task immediately even if new thread needs to be started to do so. */
		LAZY   /*!< Start execution of a task when some existing worker thread becomes available. */
	};

private:
	class HTask;                                        /*!< Forward declaration of task definition. */
	typedef yaal::hcore::HResource<HTask> task_t;       /*!< Task holder type. */
	class HWorker;                                      /*!< Forward definition of worker definition. */
	typedef yaal::hcore::HResource<HWorker> worker_t;   /*!< Working holder type. */
	typedef yaal::hcore::HList<worker_t> worker_pool_t; /*!< Type for storing set of workers. */
	typedef yaal::hcore::HList<task_t> task_queue_t;    /*!< Type for storing set of tasks. */
	int _workerPoolSize;                /*!< Maximum number of ad-hoc threads. */
	int _busyWorkers;                   /*!< Number of workers that currently run some tasks. */
	STATE _state;                       /*!< State of this thread pool, \see STATE. */
	worker_pool_t _workers;             /*!< Tasks executors. */
	task_queue_t _tasks;                /*!< Task queue. */
	yaal::hcore::HSemaphore _semaphore; /*!< Queue push/pop synchronization method. */
	yaal::hcore::HMutex _mutex;         /*!< Protect shared state from parallel access. */

public:
	/*! \brief Construct HWorkFlow object.
	 *
	 * \param workerPoolSize - maximum number of workers in this worker pool.
	 */
	HWorkFlow( int workerPoolSize = 1 );

	/*! \brief Destroy HWorkFlow object.
	 *
	 * All scheduled tasks that are running are bound to finish naturally.
	 * All scheduled tasks that not yet started are bound to start and then finish naturally.
	 */
	virtual ~HWorkFlow( void );

	/*! \brief Schedule execution of task in this worker pool.
	 *
	 * \param schedulePolicy - a schedule policy used for staring this task.
	 * \param task - task to execute by this worker pool.
	 * \param asyncStop - how to notify executing task that it should stop its execution.
	 * \param wantRestart - test telling if task should be restarted if stopped by asyncStop.
	 */
	void schedule_task( SCHEDULE_POLICY schedulePolicy, call_t task, call_t asyncStop = call_t(), want_restart_t wantRestart = want_restart_t() );

	/*! \brief Cancel scheduled tasks.
	 *
	 * \param subject - an identifier of an object on which behave task were scheduled.
	 */
	void cancel_task( void const* subject );

	/*! \brief Restart execution of tasks scheduled in this HWorkFlow.
	 *
	 * \throw HWorkFlowException on parallel start.
	 */
	void start( void );

	/*! \brief Windup all scheduled tasks.
	 *
	 * Join all currently running threads and handle
	 * scheduled tasks.
	 *
	 * Winding up can be performed in several modes (see #WINDUP_MODE).
	 *
	 * Warning: If you windup with SUSPEND or CLOSE mode and have
	 * permanently running task then your program will hang indefinitely.
	 *
	 * When this method return all threads are joined.
	 *
	 * \param windupMode - mode of tasks winding up.
	 * \throw HWorkFlowException on parallel stop.
	 */
	void windup( WINDUP_MODE windupMode );

	/*! \brief Just like windup(...) but not wait for threads to join.
	 */
	void schedule_windup( WINDUP_MODE windupMode );

	/*! \brief Join all threads.
	 */
	void join( void );

	/*! \brief Tell if calling join() would be a non-blocking call.
	 */
	bool can_join( void );

protected:
	STATE state( void ) const;

private:
	/*! \brief Pop task from task queue.
	 *
	 * \return Next task to be executer or task_t(nullptr) for no more tasks.
	 */
	task_t pop_task( void );
	void add_task_eager( call_t, call_t, want_restart_t );
	void add_task_lazy( call_t, call_t, want_restart_t );
	HWorkFlow( HWorkFlow const& ) = delete;
	HWorkFlow& operator = ( HWorkFlow const& ) = delete;
	friend class HWorker;
};

typedef yaal::hcore::HExceptionT<HWorkFlow> HWorkFlowException;

}

}

#endif /* #ifndef YAAL_TOOLS_HWORKFLOW_HXX_INCLUDED */

