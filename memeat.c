#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>

int main(int argc, char **argv)
{
	size_t amount;

	/*
	 * Parse command-line
	 */
	if (argc < 2) {
		fprintf(stderr,
			"Usage: %s amount\n"
			"Uselessly occupies the given amount of memory. Useful to reduce the amount of available page cache memory.\n"
			"By default, amount is in bytes. Following suffixes are supported: K, M, G, Ki, Mi, Gi\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Parse number */
	const char *nptr = argv[1];
	char *endptr;
	amount = strtol(nptr, &endptr, 10);

	if (endptr == nptr) {
		fprintf(stderr, "Parse error, amount must start with a number: %s\n", nptr);
		exit(EXIT_FAILURE);
	}

	/* Parse suffix */
	if (strcmp(endptr, "K") == 0)
		amount *= 1000;
	else if (strcmp(endptr, "M") == 0)
		amount *= 1000 * 1000;
	else if (strcmp(endptr, "G") == 0)
		amount *= 1000 * 1000 * 1000;
	else if (strcmp(endptr, "Ki") == 0)
		amount *= 1024;
	else if (strcmp(endptr, "Mi") == 0)
		amount *= 1024 * 1024;
	else if (strcmp(endptr, "Gi") == 0)
		amount *= 1024 * 1024 * 1024;
	else {
		fprintf(stderr, "Parse error, unrecognized suffix: %s\n", endptr);
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Will eat %ld bytes\n", amount);

	/*
	 * Eat memory
	 */

	/* Print some debugging info */
	struct rlimit rlimit;
	if (getrlimit(RLIMIT_MEMLOCK, &rlimit) == -1) {
		perror("Warning, getrlimit() failed");
	}
	else {
		fprintf(stderr, "Current RLIMIT_MEMLOCK: %ld %ld\n", rlimit.rlim_cur, rlimit.rlim_max);
		if (rlimit.rlim_cur < amount && geteuid() != 0) {
			fprintf(stderr, "Warning, this utility is run by an unprivileged user and resource limit is too low. mlock() might fail.\n");
		}
	}

	/* Actually eat */
	void *mem = mmap(NULL, amount, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED) {
		perror("Fatal error, mmap() failed");
		exit(EXIT_FAILURE);
	}
	if (mlock(mem, amount) == -1) {
		perror("Error, mlock() failed");
	}

	/*
	 * Wait
	 */
	fprintf(stderr, "Eating RAM, press CTRL+C to stop\n");
	sleep(365*24*60*60); /* XXX: 1 year instead of infinity */

	/*
	 * Cleanup
	 */
	// XXX: we rely on the kernel to clean up after us
	return EXIT_SUCCESS;
}
