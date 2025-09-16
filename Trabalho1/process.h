enum process_state {
  Running,
  Ready,
  Blocked_1,
  Blocked_2,
  Terminated,
};

typedef struct process {
  int pid;
  enum process_state state;
  int pc;
  int d1_count;
  int d2_count;
  char op_block;
} Process;