/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package rna.treinamento.util;

/**
 *
 * @author Fernando
 */
class BackNeuronio {

    double[] deltaPesos;
    double deltabias;
    double saida;
    double erro;

    public BackNeuronio() {
        deltaPesos = null;
        deltabias = 0.0;
        saida = 0.0;
        erro = 0.0;
    }

    public BackNeuronio(int nroPesos) {
        deltaPesos = new double[nroPesos];
        for (int i = 0; i < deltaPesos.length; i++) {
            deltaPesos[i] = 0.0;
        }
        deltabias = 0.0;
        saida = 0.0;
        erro = 0.0;
    }

    public BackNeuronio(double[] deltaPesos, double deltabias, double saida, double erro) {
        this.deltaPesos = deltaPesos;
        this.deltabias = deltabias;
        this.saida = saida;
        this.erro = erro;
    }

    public double getDeltaPeso(int i) {
        return deltaPesos[i];
    }

    public void setDeltaPeso(int i, double peso) {
        deltaPesos[i] = peso;
    }

//    public double[] getDeltaPesos() {
//        return deltaPesos;
//    }
//
//    public void setDeltaPesos(double[] deltaPesos) {
//        this.deltaPesos = deltaPesos;
//    }
    public double getDeltabias() {
        return deltabias;
    }

    public void setDeltabias(double deltabias) {
        this.deltabias = deltabias;
    }

    public double getSaida() {
        return saida;
    }

    public void setSaida(double saida) {
        this.saida = saida;
    }

    public double getErro() {
        return erro;
    }

    public void setErro(double erro) {
        this.erro = erro;
    }
}
