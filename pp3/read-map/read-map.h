#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <thread>
#include "read-map/read-map-worker.h"

namespace read_map {

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads) : genome_(genome), reads_(reads) {
		
	}
	~ReadMap() {

	}

	// generates A
	void PrepareST(int *A, suffix_tree::SuffixTreeNode* node, int & next_index);

	void LaunchThreads(const aligner::ScoreConfig& align_config, const suffix_tree::SuffixTree& st, const int * A, int num_threads=8) {
		cout << "Launching " << num_threads << " thread(s) ..." << endl;
		std::vector<ReadMapWorker *> workers(num_threads);
		std::vector<std::thread> threads;//(num_threads);
		int num_reads_rem = reads_.size();
		for (int tid=0; tid<num_threads; tid++) {
			int this_worker_num_reads = num_reads_rem / (num_threads - tid);
			workers[tid] = new ReadMapWorker(tid, genome_, reads_, align_config, st, A);

			threads.push_back(std::thread([worker = workers[tid]](int _istart_read, int _this_worker_num_reads) {
				cout << "_istart_read: " << _istart_read << " ; _this_worker_num_reads: " << _this_worker_num_reads << endl; 
				worker->CalcReadMappings(_istart_read, _this_worker_num_reads);
			}, (reads_.size() - num_reads_rem, this_worker_num_reads)));
			num_reads_rem -= this_worker_num_reads;
		}
		
		for (int tid=0; tid<num_threads; tid++) {
			threads[tid].join();
			delete workers[tid];
		}
	}

	void Run() {
		// generate st
		cout << endl;
		cout << "Constructing the reference genome suffix tree..." << endl;
		const suffix_tree::SuffixTree * st = new suffix_tree::SuffixTree(genome_.bps.c_str(),  genome_.bps.size());

		// generate A
		cout << endl;
		cout << "Building A (PrepareST)..." << endl;
		int next_index = 0;
		int * A = new int[genome_.bps.size()];
		PrepareST(A, st->root_, next_index);
		
		// do the work
		cout << endl;
		cout << "Calculating the read mappings..." << endl;
		const aligner::ScoreConfig align_config = {1, -2, -5, -1};
		LaunchThreads(align_config, *st, A);

		delete[] A;
		delete st;
	}

	Sequence & genome_;
	std::vector<Sequence> & reads_;
};

} // namespace read_map

#endif