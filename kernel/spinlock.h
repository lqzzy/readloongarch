
struct spinlock {
	int locked;
	struct cpu *cpu;
	char *name;
};

