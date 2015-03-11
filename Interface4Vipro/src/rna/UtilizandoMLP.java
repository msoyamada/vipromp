package rna;

import java.text.DecimalFormat;
import rna.treinamento.BackPropagationMLP;
import rna.redes.MLP;

public class UtilizandoMLP {

    public static void main(String[] args) {
        double[][] entrada = {{1, 1}, {1, 0}, {0, 1}, {0, 0}};
        double[][] saidaDesejada = {{0}, {1}, {1}, {0}};

        //nro de epocas é quantidade de vezes que todo o conjunto de treinamento
        //vai ser apresentado para a rede. Neste caso o conjunto
        //{{1, 1}, {1, 0}, {0, 1}, {0, 0} vai ser apresentado 2000 vezes para
        //a rede
        int epocas = 10000;

        //taxa de aprendizado é a velocidade com que a rede vai atualizar o peso
        //dos neurônio
        double taxaAprendizagem = 0.4;
        //momento tambem serve para acelerar o treinamento
        double momento = 0.3;

        //isto nao tem nada a ver com a rede, é só para formatar a saida do resultado
        //dela
        DecimalFormat df = new DecimalFormat();
        df.setMaximumFractionDigits(10);
        df.setMinimumFractionDigits(10);

        //crio a rede MLP
        //o construtor recebe tres parametros:
        // -> o primeiro indica quantos neurônios tem a camada de entrada
        // -> o segundo indica quantas camada ocultas tem a rede e quantos neurônios tem cada camada
        // -> o terceiro indica quantos neurônio tem a camada de saida
        //Por exemplo se você quer uma rede com 3 entradas, 2 camadas ocultas com
        //5 neurônios cada uma e 1 saida, o construtor vai ficar new MLP (3, "5 5", 1).
        //Se você quer um rede com 20 entradas , 1 camada oculta de 10 neurônios
        //e duas saidas o contrutor vai ficar new MLP(20,"10",2), e assim por diante.
        //Pelo que vc me pediu no e-mail a rede tem que ter 45 entradas, 2 camadas ocultas
        //N saidas, então o construtor vai ficar new MLP(45,"A B",N), onde A é o número
        //de neurônios da primeira camada oculta e B é o número de neurônios da segunda
        //camada oculta, não tem formula para descobrir os valores de A e B
        //tem que testar um monte de valores ate achar uma que de bons resultados
        //Aqui em baixo eu estou criando uma rede com 2 entradas, 5 neurônios na
        //primeira camada oculta, 10 neurônios na segunda camada oculta e 1 neurônio
        //na camada de saída.
        MLP mlp = new MLP(2, "50", 1);

        //crio um objetto do tipo backpropagation para treinar a rede criada
        BackPropagationMLP bc = new BackPropagationMLP(mlp, taxaAprendizagem, momento);
        for (int i = 0; i < epocas; i++) {
            for (int j = 0; j < entrada.length; j++) {
                //treina a rede com cada entrada disponivel
                bc.treinar(entrada[j], saidaDesejada[j]);
                double[] saida = mlp.calcular(entrada[j]);
                for (int k = 0; k < saida.length; k++) {
                    //imprime a saida verdadeira e a saida calculada pela rede
                    System.out.print(df.format(saidaDesejada[j][k]) + " : " + df.format(saida[k]) + " | ");
                }
                System.out.println("");
            }
        }

    }
}
