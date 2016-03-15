#ifndef TEST_IRQ_H
#define TEST_IRQ_H

void call_interrupt_handlers(const char *fn, unsigned int line);
void execute_completions(const char *fn, unsigned int line);

#endif
