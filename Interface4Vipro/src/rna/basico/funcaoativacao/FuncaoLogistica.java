package rna.basico.funcaoativacao;

public class FuncaoLogistica implements FuncaoAtivacao {

    public double calcular(double valor) {
        return (1 / (1 + Math.exp(-valor))); //logsig
        //return (2 * Math.exp(2 * valor)); //tansig
    }

    public double derivada(double valor) {
        return valor * (1 - valor); //logsig
        //return (2 * Math.exp(2 * valor)); //tansig
    }
}
