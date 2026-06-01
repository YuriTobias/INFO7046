# TRABALHO PRÁTICO 1: Dois Algoritmos para Eleição Distribuída de Líder em Anel

Atenção: a data limite para disponibilizar o trabalho é 31 de maio de 2026 (não serão aceitos trabalhos fora do prazo!) Atenção: são mais de 3 semanas, mas organize-se pois o tempo é curto para fazer todo o necessário

Os alunos devem informar por e-mail a URL do trabalho, usando o subject "TP SISDIS 2026-1"

Os trabalhos de alunos de graduação devem ser feitos em dupla, alunos do PPGInf: trabalhos individuais.

## ALGORITMO 1: Implemente o algoritmo de eleição de líder com candidatos baseado em Chang-Roberts no anel (VRing). Neste algoritmo inicialmente são definidos quais processos (com identificador 0..N-1) são candidatos a líder. Todo processo mantém uma variável local Lider na qual armazenda o id do processo que considera líder. Se o processo é candidato, já assinala seu próprio id para Lider na inicialização. Um não candidato assinala o valor -1 na inicialização. Um processo que é candidato manda uma mensagem para o seguinte informando seu id e que é candidato. Vamos usar como critério para a eleição o maior identificador. Assim, se um processo recebe uma mensagem de um candidato com id maior que aquele indicado pela variável Lider, modifica o valor e repassa para frente no anel a mensagem recebida. Quando um processo recebe de volta sua própria mensagem informando que é candidato, sabe que é o líder e todos o processos do anel já receberam sua mensagem. O processo pode então descartar a mensagem e executar procedimento de lider. Mostre no log execuções do algoritmo para 1 único candidato, vários candidatos selecionados aleatoriamente e todos os N processos inicialmente candidatos. Destaque para cada execução quem é o líder eleito. Conte quantas mensagens foram necessárias em cada caso e também quanto tempo (do SMPL) foi necessário para executar a eleição.

## ALGORITMO 2: Implemente o algoritmo aleatorizado para eleição de líder (Randomized Leader Election Algorithm) baseado no anel (VRing). Neste algoritmo cada processo sorteia, em cada rodada um bit aleatório. O algoritmo funciona em rodadas. Na primeira rodada todos os N processos sorteiam um valor aleatório para o seu bit. Os processos com bit = 1 são candidatos, os demais processos (i.e. com seu bit = 0) não são candidatos. Cada processo deve enviar uma mensagem que roda todo o anel informando seu id e seu bit da rodada. Na rodada seguinte, apenas os processos com bit = 1 sorteiam novo valor aleatório para o bit. Esses processos então comunicam novamente no anel o valor dos seus bits. Novas rodadas vão acontecendo até que 1 processo se transforme em líder. Execute o algoritmo para diferentes valores de N. Destaque para cada execução quem é o líder eleito. Conte quantas mensagens foram necessárias em cada caso e também quanto tempo (do SMPL) foi necessário para executar a eleição.

Deve ser feita uma página Web, que contém:

- Relatório HTML explicando o algoritmo proposto, e descrevendo todos os itens solicitados acima (use desenhos, palavras, o que você quiser): o objetivo é detalhar as suas decisões para implementar seu trabalho.
- Código fonte dos programas, comentados. ATENÇÃO: acrescente a todo programa a terminação ".txt" para que possa ser diretamente aberto em um browser. Exemplos: cliente.py.txt ou servidor.c.txt
- Log dos testes executados: mostre explicitamente diversos casos testados, lembre-se é a partir desta listagem de testes que o professor vai medir até que ponto o trabalho está funcionando.
  Veja este programa exemplo para ilustrar a criação de um bom log.

# TRABALHO PRÁTICO 0: Roteiro de Aprendizado Prático de Simulação

Tarefas para aprender a usar nossa ferramenta de simulação, o SMPL.

- Tarefa 0: digitar, compilar e executar o programa exemplo, tempo.c

- Tarefa 1: Fazer cada um dos processos testar o seguinte no anel. Implemente o teste com a função status() do SMPL e imprimir (printf) o resultado de cada teste executado. Por exemplo: “O processo i testou o processo j correto no tempo tal.”

- Tarefa 2: Cada processo correto executa testes até achar outro processo correto. Lembre-se de tratar o caso em que todos os demais processos estão falhos. Imprimir os testes e resultados.

- Tarefa 3: Cada processo mantém localmente o vetor State[N]. A entrada do vetor State[j] indica o estado do processo j. O estado de cada processo pode ser: -1 (unknown), 0 (correto) ou 1 (falho). Inicialize (para todos os processos) o State[N] com -1 (indicando estado “unknown”) para todos os demais processos e 0 para o próprio processo. Nesta tarefa ao executar um teste em um processo j, o testador atualiza a entrada correspondente no vetor State[j]. Em cada intervalo de testes, mostre o vetor State[N].

- Tarefa 4: Quando um processo correto testa outro processo correto obtém as informações do estado dos demais processos do sistema, processos do sistema exceto aqueles que testou nesta rodada, além do próprio testador.

Na página Web do Trabalho Prático, por favor incluam links para as 5 tarefas acima.
