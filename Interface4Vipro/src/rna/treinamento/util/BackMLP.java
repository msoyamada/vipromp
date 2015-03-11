/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package rna.treinamento.util;

import java.util.ArrayList;
import rna.redes.MLP;

/**
 *
 * @author Fernando
 */
public class BackMLP {

    ArrayList<BackCamada> listaBackCamadas;

    public BackMLP(MLP mlp) {
        listaBackCamadas = new ArrayList<BackCamada>();
        int nroCamadas = mlp.nroCamadas();
        for (int i = 0; i < nroCamadas; i++) {
            int nroNeuronios = mlp.nroNeuroniosCamada(i);
            int nroPesos = mlp.nroPesosNeuroniosCamada(i);
            listaBackCamadas.add(new BackCamada(nroNeuronios, nroPesos));
        }
    }

    public double getDeltaPeso(int nroCamada, int nroNeuronio, int i) {
        return listaBackCamadas.get(nroCamada).getDeltaPeso(nroNeuronio, i);
    }

    public void setDeltaPeso(int nroCamada, int nroNeuronio, int i, double peso) {
        listaBackCamadas.get(nroCamada).setDeltaPeso(nroNeuronio, i, peso);
    }

    public double getDeltaBias(int nroCamada, int nroNeuronio) {
        return listaBackCamadas.get(nroCamada).getDeltaBias(nroNeuronio);
    }

    public void setDeltaBias(int nroCamada, int nroNeuronio, double deltaBias) {
        listaBackCamadas.get(nroCamada).setDeltaBias(nroNeuronio, deltaBias);
    }

    public double getSaida(int nroCamada, int nroNeuronio) {
        return listaBackCamadas.get(nroCamada).getSaida(nroNeuronio);
    }

    public void setSaida(int nroCamada, int nroNeuronio, double valor) {
        listaBackCamadas.get(nroCamada).setSaida(nroNeuronio, valor);
    }

    public double getErro(int nroCamada, int nroNeuronio) {
        return listaBackCamadas.get(nroCamada).getErro(nroNeuronio);
    }

    public void setErro(int nroCamada, int nroNeuronio, double valor) {
        listaBackCamadas.get(nroCamada).setErro(nroNeuronio, valor);
    }
}
