/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package rna.treinamento.util;

import java.util.ArrayList;

/**
 *
 * @author Fernando
 */
class BackCamada {

    ArrayList<BackNeuronio> listaBackNeuronios;

    public BackCamada() {
        listaBackNeuronios = new ArrayList<BackNeuronio>();
    }

    public BackCamada(int nroNeuronios, int nroPesos) {
        listaBackNeuronios = new ArrayList<BackNeuronio>();
        for (int i = 0; i < nroNeuronios; i++) {
            listaBackNeuronios.add(new BackNeuronio(nroPesos));
        }
    }

    public BackCamada(ArrayList<BackNeuronio> listaNeuronios) {
        this.listaBackNeuronios = listaNeuronios;
    }

    public BackNeuronio getBackNeuronio(int i) {
        return listaBackNeuronios.get(i);
    }

    public void setBackNeuronio(int i, BackNeuronio backNeuronio) {
        listaBackNeuronios.set(i, backNeuronio);
    }

    public double getDeltaPeso(int neuronio, int i) {
        return listaBackNeuronios.get(neuronio).getDeltaPeso(i);
    }

    public void setDeltaPeso(int neuronio, int i, double peso) {
        listaBackNeuronios.get(neuronio).setDeltaPeso(i, peso);
    }

    public double getDeltaBias(int neuronio) {
        return listaBackNeuronios.get(neuronio).getDeltabias();
    }

    public void setDeltaBias(int neuronio, double peso) {
        listaBackNeuronios.get(neuronio).setDeltabias(peso);
    }

    public double getSaida(int neuronio) {
        return listaBackNeuronios.get(neuronio).getSaida();
    }

    public void setSaida(int neuronio, double saida) {
        listaBackNeuronios.get(neuronio).setSaida(saida);
    }

    public double getErro(int neuronio){
        return listaBackNeuronios.get(neuronio).getErro();
    }

    public void setErro(int neuronio, double erro){
        listaBackNeuronios.get(neuronio).setErro(erro);
    }
//    public ArrayList<BackNeuronio> getListaNeuronios() {
//        return listaBackNeuronios;
//    }
//
//    public void setListaNeuronios(ArrayList<BackNeuronio> listaNeuronios) {
//        this.listaBackNeuronios = listaNeuronios;
//    }
}