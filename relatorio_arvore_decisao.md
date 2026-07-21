# Relatório de Implementação: Árvore de Decisão

Este documento detalha as principais ideias, decisões arquiteturais e técnicas adotadas para a implementação do modelo de Árvore de Decisão em C++ nesta biblioteca.

## 1. Arquitetura, Estruturas de Dados e Gerenciamento de Memória
- **Orientação a Objetos e Abstração:** O algoritmo foi projetado adotando os princípios de Orientação a Objetos. A classe `DecisionTree` herda de uma interface base `Classifier`, que abstrai métodos fundamentais de aprendizado de máquina como `fit` e `predict`. Essa arquitetura permite que outros classificadores sejam facilmente introduzidos no ecossistema sem alterar a interface esperada.
- **Gerenciamento Seguro de Memória:** Para modelar a estrutura hierárquica da árvore, utilizou-se o `struct TreeNode`. As conexões pai-filho (`left` e `right`) são feitas exclusivamente utilizando ponteiros inteligentes (`std::shared_ptr<TreeNode>`). Esta foi uma decisão deliberada para garantir segurança no gerenciamento de memória e evitar vazamentos (memory leaks) durante a destruição e sobreposição de árvores.
- **Tipagem Customizada:** Utilizou-se aliases léxicos (ex: `using Matrix = std::vector<std::vector<FeatureType>>` e `using Labels = std::vector<LabelType>`) para dar mais clareza e legibilidade aos tipos de dados operados pela árvore.

## 2. Construção da Árvore e Particionamento
- **Abordagem Recursiva Gulosa:** O treinamento (método `fit` e rotina interna `build_tree`) utiliza uma estratégia top-down. Em cada etapa da árvore, o algoritmo realiza partições binárias exaustivas para dividir os dados, repetindo o processo recursivamente para os subconjuntos da esquerda e da direita.
- **Critério de Qualidade (Impureza de Gini):** O método adotado para avaliar a qualidade dos cortes é o Índice de Gini (`calculate_gini`). A busca pelo melhor corte (`find_best_split`) tenta minimizar a impureza dos nós filhos (ou seja, maximizar o ganho de informação baseado no Gini de probabilidade das classes).
- **Busca Linear de Limiares (Thresholds):** A avaliação do ponto de corte ideal avalia de maneira iterativa cada valor de característica (feature) das amostras de treino para identificar qual fornece o maior ganho de separabilidade entre as classes, tornando o algoritmo flexível aos dados fornecidos sem pressupor sua distribuição.

## 3. Critérios de Parada e Prevenção de Overfitting
Para garantir a capacidade de generalização do classificador, evitando que a árvore "memorize" os dados (overfitting), a implementação conta com critérios de parada estritos durante a recursão:
- **Profundidade Máxima (`max_depth`):** A estrutura da árvore para de crescer ao atingir um nível máximo de camadas pré-estabelecido.
- **Ganho Mínimo (`min_gain`):** Se o ganho de Gini após um possível corte não superar este patamar mínimo, o nó não é dividido, convertendo-se precocemente em uma folha.
- **Voto Majoritário (`calculate_majority_vote`):** Ao ativar um critério de parada, o nó atual vira um nó folha (`is_leaf = true`). Para definir a qual classe ele responderá, foi adotado um sistema de Voto Majoritário simples, contabilizando a frequência das classes daquele conjunto residual e assinalando a predição para a classe dominante.

## 4. Persistência de Dados (Serialização do Modelo)
- **Carga e Salvamento em Disco (`save` e `load`):** Foi implementada a funcionalidade de gravar a árvore treinada em um arquivo texto para uso futuro.
- **Travessia Pré-Ordem:** Para serializar a estrutura não-linear de árvore num arquivo plano, os nós são guardados recursivamente em pré-ordem estrutural.
  - Nós de decisão (internos) são prefixados com a letra `I`, seguidos do índice da característica e do limiar.
  - Nós finais (folhas) são prefixados com a letra `L`, contendo apenas o inteiro da classe predita.
- **Benefício:** Essa formatação enxuta e textual permite que a reconstrução na leitura seja imediata, decodificando o tipo do nó pela primeira letra e chamando a alocação de sub-árvores recursivamente sem perda estrutural.

## 5. Avaliação do Modelo (Métricas)
Para validar o desempenho do modelo (em particular para o problema de autenticação/classificação binária, onde a classe 1 é a voz/alvo de interesse e 0 as demais), foi elaborada uma classe dedicada `Metrics`. As avaliações ocorrem a partir da comparação de `y_true` (rótulos reais) e `y_pred` (rótulos preditos):

- **Matriz de Confusão:** É calculada logo na instanciação da classe e tabula as 4 combinações possíveis:
  - **TP (True Positive - Verdadeiros Positivos):** Quando a classe real é 1 e o modelo acerta (prediz 1).
  - **TN (True Negative - Verdadeiros Negativos):** Quando a classe real é 0 e o modelo acerta (prediz 0).
  - **FP (False Positive - Falsos Positivos):** Quando a classe real é 0, mas o modelo prediz incorretamente como 1 (falso alarme).
  - **FN (False Negative - Falsos Negativos):** Quando a classe real é 1, mas o modelo prediz incorretamente como 0 (não reconhecimento).

A partir dessa matriz, as seguintes métricas de desempenho são derivadas e podem ser exibidas através do método `printReport()`:
- **Acurácia (`accuracy`):** A proporção total de acertos (`TP + TN`) pelo total de predições. Mede o percentual geral de acertos do modelo.
- **Precisão (`precision`):** Calculada por `TP / (TP + FP)`. Responde à pergunta: de todas as instâncias que o modelo classificou como a classe de interesse (ex: "é a sua voz"), quantas realmente eram?
- **Recall/Sensibilidade (`recall`):** Calculado por `TP / (TP + FN)`. Responde à pergunta: de todas as instâncias que eram realmente da classe de interesse na base, quantas o modelo conseguiu de fato encontrar?
- **F1-Score (`f1_score`):** A média harmônica entre a Precisão e o Recall (`2 * (prec * rec) / (prec + rec)`). Trata-se de uma métrica muito mais robusta que a acurácia para bases de dados onde há desbalanceamento entre as classes, forçando um equilíbrio entre Falsos Positivos e Falsos Negativos.
