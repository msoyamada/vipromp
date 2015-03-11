package rna.treinamento;

import rna.redes.MLP;
import rna.treinamento.util.BackMLP;

public class BackPropagationMLP {

    MLP mlp;
    BackMLP backMLP;
    double taxaAprendizagem;
    double momento;

    public BackPropagationMLP() {
        mlp = null;
        backMLP = null;
        taxaAprendizagem = 0.0;
        momento = 0.0;
    }

    public BackPropagationMLP(MLP mlp, double taxaAprendizagem, double momento) {
        this.mlp = mlp;
        this.backMLP = new BackMLP(mlp);
        this.taxaAprendizagem = taxaAprendizagem;
        this.momento = momento;
    }

    public void treinar(double[] entrada, double[] saidaDesejada) {

        /* TESTE PARA VER SE ESTOU MANDANDO ARQUITETURAS DIFERENTES
        String aux1 = "";
        for (int y = 0; y < entrada.length; y++) aux1 += entrada[y] + " ";
        System.out.println("PoTencia: " + aux1);*/

        //calcula a saida de cada neuronio e armazenar resultado em listaBackCamadas
        computacaoParaFrente(entrada);
        retropropagacao(entrada, saidaDesejada);
    }

    private void computacaoParaFrente(double[] entrada) {
        int nroCamadas = mlp.nroCamadas();
        for (int i = 0; i < nroCamadas; i++) {
            int nroNeuronios = mlp.nroNeuroniosCamada(i);
            double[] aux = new double[nroNeuronios];
            for (int j = 0; j < nroNeuronios; j++) {
                aux[j] = mlp.sinalFuncional(i, j, entrada);
                backMLP.setSaida(i, j, aux[j]);
            }
            entrada = aux;
        }
    }

    private void retropropagacao(double[] entrada, double[] saidaDesejada) {
        calculaErroCamadaSaida(saidaDesejada);
        calculaErroCamadasOcultas();
        calculaDeltas(entrada);
        corrigeDeltas();
    }

    private void calculaErroCamadaSaida(double[] saidaDesejada) {
        int indiceCamadaSaida = mlp.nroCamadas() - 1;
        int nroNeuroniosCamadaSaida = mlp.nroNeuroniosCamada(indiceCamadaSaida);
        for (int i = 0; i < nroNeuroniosCamadaSaida; i++) {
            double derivada = mlp.getFuncaoAtivacao(indiceCamadaSaida, i).derivada(backMLP.getSaida(indiceCamadaSaida, i));
            double erro = derivada * (saidaDesejada[i] - backMLP.getSaida(indiceCamadaSaida, i));
            backMLP.setErro(indiceCamadaSaida, i, erro);
        }
    }

    private void calculaErroCamadasOcultas() {
        int indiceUltimaCamadaOculta = mlp.nroCamadas() - 2;
        for (int i = indiceUltimaCamadaOculta; i >= 0; i--) {
            int nroNeuronios = mlp.nroNeuroniosCamada(i);
            for (int j = 0; j < nroNeuronios; j++) {
                double derivada = mlp.getFuncaoAtivacao(i, j).derivada(backMLP.getSaida(i, j));
                double somatorioK = 0.0;
                for (int k = 0; k < mlp.nroNeuroniosCamada(i + 1); k++) {
                    somatorioK += backMLP.getErro(i + 1, k) * mlp.getPeso(i + 1, k, j);
                }
                double erro = derivada * somatorioK;
                backMLP.setErro(i, j, erro);
            }
        }
    }

    private void calculaDeltas(double[] entrada) {
        for (int j = 0; j < mlp.nroNeuroniosCamada(0); j++) {
            for (int k = 0; k < mlp.nroPesosNeuroniosCamada(0); k++) {
                double delta = taxaAprendizagem * backMLP.getErro(0, j) * entrada[k];
                delta+=momento*backMLP.getDeltaPeso(0, j, k);
                backMLP.setDeltaPeso(0, j, k, delta);
            }

            backMLP.setDeltaBias(0, j, momento*backMLP.getDeltaBias(0, j)+ taxaAprendizagem * backMLP.getErro(0, j));
        }

        int nroCamadas = mlp.nroCamadas();
        for (int i = 1; i < nroCamadas; i++) {
            int nroNeuronios = mlp.nroNeuroniosCamada(i);
            for (int j = 0; j < nroNeuronios; j++) {
                int nroPesos = mlp.nroPesosNeuroniosCamada(i);
                for (int k = 0; k < nroPesos; k++) {
                    double delta = taxaAprendizagem * backMLP.getErro(i, j) * backMLP.getSaida(i-1, k);
                    delta+=momento*backMLP.getDeltaPeso(i, j, k);
                    backMLP.setDeltaPeso(i, j, k, delta);
                }
                backMLP.setDeltaBias(i, j,  momento*backMLP.getDeltaBias(i, j)+taxaAprendizagem * backMLP.getErro(i, j));
            }
        }
    }

    private void corrigeDeltas() {
        int nroCamadas = mlp.nroCamadas();
        for (int i = 0; i < nroCamadas; i++) {
            int nroNeuronios = mlp.nroNeuroniosCamada(i);
            for (int j = 0; j < nroNeuronios; j++) {
                int nroPesos = mlp.nroPesosNeuroniosCamada(i);
                for (int k = 0; k < nroPesos; k++) {
                    mlp.setPeso(i, j, k, mlp.getPeso(i, j, k)+backMLP.getDeltaPeso(i, j, k));
                }
                mlp.setBias(i,j,mlp.getBias(i, j)+backMLP.getDeltaBias(i, j));
            }
        }
    }
}