package rna.redes;

import java.util.ArrayList;
import rna.basico.Camada;
import rna.basico.funcaoativacao.FuncaoAtivacao;

public class MLP {

    int nroNeuroniosEntrada;
    ArrayList<Camada> listaCamadas;

    public MLP() {
        nroNeuroniosEntrada = 0;
        listaCamadas = new ArrayList<Camada>();
    }

    public MLP(int nroEntradas) {
        this.nroNeuroniosEntrada = nroEntradas;
        listaCamadas = new ArrayList<Camada>();
    }

    public MLP(int nroEntradas, ArrayList<Camada> listaCamadas) {
        this.nroNeuroniosEntrada = nroEntradas;
        this.listaCamadas = listaCamadas;
    }

    public MLP(int nroNeuroniosCamadaEntrada, String neuroniosCamadasOcultas, int nroNeuroniosCamadaSaida) {
        this.nroNeuroniosEntrada = nroNeuroniosCamadaEntrada;
        listaCamadas = new ArrayList<Camada>();
        int nroPesos = nroNeuroniosCamadaEntrada;

        if (neuroniosCamadasOcultas!=null&&neuroniosCamadasOcultas.compareTo("")!=0) {
            String[] cam = neuroniosCamadasOcultas.split(" ");

            for (int i = 0; i < cam.length; i++) {
                listaCamadas.add(new Camada(Integer.parseInt(cam[i]), nroPesos));
                nroPesos = Integer.parseInt(cam[i]);
            }
        }

        listaCamadas.add(new Camada(nroNeuroniosCamadaSaida, nroPesos));
    }

    public Camada getCamada(int indice) {
        return listaCamadas.get(indice);
    }

    public void setCamada(int indice, Camada camada) {
        listaCamadas.set(indice, camada);
    }

//    public Neuronio getNeuronio(int camada, int neuronio) {
//        return listaCamadas.get(camada).getNeuronio(neuronio);
//    }
//
//    public void setNeuronio(int camada, int i, Neuronio neuronio) {
//        listaCamadas.get(camada).setNeuronio(i, neuronio);
//    }
    public double getPeso(int camada, int neuronio, int i) {
        return listaCamadas.get(camada).getPesoNeuronio(neuronio, i);
    }

    public void setPeso(int camada, int neuronio, int i, double peso) {
        listaCamadas.get(camada).setPesoNeuronio(neuronio, i, peso);
    }

    public int nroCamadas() {
        return listaCamadas.size();
    }
//    public ArrayList<Camada> getListaCamadas() {
//        return listaCamadas;
//    }
//
//    public void setListaCamadas(ArrayList<Camada> listaCamadas) {
//        this.listaCamadas = listaCamadas;
//    }

    public int getNroEntradas() {
        return nroNeuroniosEntrada;
    }

    public void setNroEntradas(int nroEntradas) {
        this.nroNeuroniosEntrada = nroEntradas;
    }

    //adiciona uma camada na saida da rede
    public void adicionarLayer(int nroNeuronios) {
        if (listaCamadas.size() == 0) {
            listaCamadas.add(new Camada(nroNeuronios, this.nroNeuroniosEntrada));
        } else {
            //o numero de pesos de cada neuronio sera igual ao numero de
            //neuronios da ultima camada            
            listaCamadas.add(new Camada(nroNeuronios, listaCamadas.get(listaCamadas.size() - 1).nroNeuronios()));

        }
    }

    public double[] calcular(double[] entrada) {

        /* TESTE PARA VER SE ESTOU MANDANDO ARQUITETURAS DIFERENTES
        String aux1 = "";
        for (int y = 0; y < entrada.length; y++) aux1 += entrada[y] + " ";
        System.out.println(aux1);*/

        if (entrada.length == nroNeuroniosEntrada) {
            for (int i = 0; i < listaCamadas.size(); i++) {
                Camada c = listaCamadas.get(i);

                //verifica se o tamanho da entrada e igual ao numero de pesos
                //dos neuronios da camada
                if (c.nroPesosNeuronios() == entrada.length) {
                    double[] aux = new double[c.nroNeuronios()];
                    for (int j = 0; j < c.nroNeuronios(); j++) {
                        aux[j] = c.sinalFuncionalNeuronio(j, entrada);
                    }
                    entrada = aux;
                } else {
                    System.out.println("Erro: tamanho da entrada diferente "
                            + "do numero de pesos dos neuronios da camada");
                    return null;
                }

            }
        } else {
            System.out.println("Tamanho da entrada passada como parametro diferente"
                    + " do tamanho da entrada da rede ");
            System.out.println("Tamanho entrada rede: " + nroNeuroniosEntrada);
            System.out.println("Tamanho entrada: " + entrada.length);

        }
        return entrada;
    }

    public int nroNeuroniosCamada(int camada) {
        return listaCamadas.get(camada).nroNeuronios();
    }

    public int nroPesosNeuroniosCamada(int camada) {
        return listaCamadas.get(camada).nroPesosNeuronios();
    }

    public double sinalFuncional(int nroCamada, int nroNeuronio, double[] entrada) {
        return listaCamadas.get(nroCamada).sinalFuncionalNeuronio(nroNeuronio, entrada);
    }

    public FuncaoAtivacao getFuncaoAtivacao(int nroCamada, int nroNeuronio) {
        return listaCamadas.get(nroCamada).getFuncaoAtivacao(nroNeuronio);
    }

    public double getBias(int nroCamada, int nroNeuronio) {
        return listaCamadas.get(nroCamada).getBias(nroNeuronio);
    }

    public void setBias(int nroCamada, int nroNeuronio, double valor) {
        listaCamadas.get(nroCamada).setBias(nroNeuronio, valor);
    }

    @Override
    public String toString() {
        String s = "";
        for (int i = 0; i < listaCamadas.size(); i++) {
            s += "Camada " + i + " \n" + listaCamadas.get(i).toString() + "\n";
        }
        return s;
    }
}
