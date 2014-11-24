README
======
To run the comparison between different priority queue implementations (std, lock-based, lock-free):
Usage <method> <threads> <inserts> <fixedPercentage>
* Method to be run: pop|remove
* Threads to be used
* Number of insert operations
* Fixed percentage of inserts

Examples:
sh run.sh remove 100 100 0.1
sh run.sh remove 100 100 0.1
