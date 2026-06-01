SRC_DIR = src

SUPPORT_OBJS = smpl.o rand.o

TARGETS = chang_roberts randomized

all: $(TARGETS)

# 1. ATUALIZAÇÃO: O queue.o agora é local (raiz), não tem o $(SRC_DIR)/ antes dele
randomized: $(SRC_DIR)/randomized.o queue.o $(SUPPORT_OBJS)
	$(LINK.c) -o $@ -Bstatic $^ -lm

# Regra geral para os outros alvos
%: $(SRC_DIR)/%.o $(SUPPORT_OBJS)
	$(LINK.c) -o $@ -Bstatic $^ -lm

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(COMPILE.c) -g -o $@ $<

%.o: %.c
	$(COMPILE.c) -g -o $@ $<

smpl.o: smpl.h
$(SRC_DIR)/tempo.o: smpl.h
$(SRC_DIR)/tarefa0.o: smpl.h
$(SRC_DIR)/tarefa1.o: smpl.h
$(SRC_DIR)/tarefa2.o: smpl.h
$(SRC_DIR)/tarefa3.o: smpl.h
$(SRC_DIR)/tarefa4.o: smpl.h
$(SRC_DIR)/chang_roberts.o: smpl.h

# 2. ATUALIZAÇÃO: Mapeamento dos cabeçalhos na raiz e no SRC_DIR
queue.o: queue.h
$(SRC_DIR)/randomized.o: smpl.h queue.h

# Limpeza
clean:
	$(RM) *.o $(SRC_DIR)/*.o $(TARGETS) relat saida