package rna.basico;

import java.util.ArrayList;
import rna.basico.funcaoativacao.FuncaoAtivacao;

public class Camada {

    ArrayList<Neuronio> listaNeuronios;

    public Camada() {
        listaNeuronios = new ArrayList<Neuronio>();
    }

    public Camada(int nroNeuronios, int nroPesos) {
        listaNeuronios = new ArrayList<Neuronio>();
        for (int i = 0; i < nroNeuronios; i++) {
            listaNeuronios.add(new Neuronio(nroPesos));
        }
    }

    public Camada(ArrayList<Neuronio> listaNeuronios) {
        this.listaNeuronios = listaNeuronios;
    }

    public int nroNeuronios() {
        return listaNeuronios.size();
    }

    public int nroPesosNeuronios() {
        return listaNeuronios.get(0).nroPesos();
    }

//    public Neuronio getNeuronio(int indice) {
//        return listaNeuronios.get(indice);
//    }

//    public void setNeuronio(int indice, Neuronio neuronio) {
//        listaNeuronios.set(indice, neuronio);
//    }

    public double getPesoNeuronio(int indiceNeuronio, int indicePeso) {
        return listaNeuronios.get(indiceNeuronio).getPeso(indicePeso);
    }

    public void setPesoNeuronio(int indiceNeuronio, int indicePeso, double peso) {
        listaNeuronios.get(indiceNeuronio).setPeso(indicePeso, peso);
    }

    public double sinalFuncionalNeuronio(int neuronio,double [] entrada){
        return listaNeuronios.get(neuronio).sinalFuncional(entrada);
    }

    public FuncaoAtivacao getFuncaoAtivacao(int nroNeuronio){
        return listaNeuronios.get(nroNeuronio).getFuncaoAtivacao();
    }

    public double getBias(int nroNeuronio){
        return listaNeuronios.get(nroNeuronio).getBias();
    }

    public void setBias(int nroNeuronio, double valor){
        listaNeuronios.get(nroNeuronio).setBias(valor);
    }

//    public ArrayList<Neuronio> getListaNeuronios() {
//        return listaNeuronios;
//    }
//
//    public void setListaNeuronios(ArrayList<Neuronio> listaNeuronios) {
//        this.listaNeuronios = listaNeuronios;
//    }
    @Override
    public String toString() {
        String saida = "[\n";
        for (int i = 0; i < listaNeuronios.size(); i++) {
            saida += listaNeuronios.get(i).toString() + "\n";
        }
        saida += "]";
        return saida;
    }
}
