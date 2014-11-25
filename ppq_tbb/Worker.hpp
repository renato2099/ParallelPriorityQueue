#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
class Worker 
{
	priority_queue<int>* ppq;
	int id;
	int numInserts;
	float fixInserts;

	/*...*/
    void runComputeHeavyOperation(int i);
    
    void computeHeavyMethod() {
        /*...*/
        tbb::task_scheduler_init init(1);
        TbbExecutor tbbExec(this);
        tbb::parallel_for(tbb::blocked_range<size_t>(0,1),tbbExec);
    }
    /*...*/

private:
    struct TbbExecutor 
    {
    public:
        TbbExecutor(Worker* w) : w_(w) {}

        void operator() (const tbb::blocked_range<size_t> r) const {
            for (size_t i = r.begin(); i!=r.end(); ++i) {
                w_->runComputeHeavyOperation(i); 
            }
        }

    private:
        Worker* w_;
    };
};