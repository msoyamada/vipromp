/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package controlador;

import modelo.Ambiente;

/**
 *
 * @author alunos
 */
public class Controlador {

    public static String controlador(String Comando){

        String[] aux = Comando.split(":&:");

        //for (int i = 0; i < aux.length; i++) System.out.println(i + " " + aux[i]);

        if (aux[aux.length - 1].equals("MakeFile"))                     return Ambiente.makeFile(Comando);
        else if (aux[aux.length - 1].equals("Main"))                    return Ambiente.Main(Comando);
        else if (aux[aux.length - 1].equals("Script"))                  return Ambiente.Scripts(Comando);
        else if (aux[aux.length - 1].equals("Configure"))               return Ambiente.makeConfigure(Comando);
        else if (aux[aux.length - 1].equals("GetFiles"))                return Ambiente.GetFiles(Comando);
        else if (aux[aux.length - 1].equals("CommandMake"))             return Ambiente.CommandMake(Comando);
        else if (aux[aux.length - 1].equals("Executar"))                return Ambiente.Executar(Comando);

        else if (aux[aux.length - 1].equals("GetIn"))                   return Ambiente.GetIn(Comando);
        else if (aux[aux.length - 1].equals("GetSelectType"))           return Ambiente.GetSelectType(Comando);
        else if (aux[aux.length - 1].equals("GetSelectCarac"))          return Ambiente.GetSelectCarac(Comando);

        else if (aux[aux.length - 1].equals("zeraConect"))              return Ambiente.zeraConect(Comando);
        else if (aux[aux.length - 1].equals("descelecionar"))           return Ambiente.descelecionar(Comando);
        else if (aux[aux.length - 1].equals("selecionar"))              return Ambiente.selecionar(Comando);
        else if (aux[aux.length - 1].equals("setLocal"))                return Ambiente.setLocal(Comando);
        
        else if (aux[aux.length - 1].equals("newDir"))                  return Ambiente.newDir(Comando);
        else if (aux[aux.length - 1].equals("Save"))                    return Ambiente.Save(Comando);
        else if (aux[aux.length - 1].equals("Open"))                    return Ambiente.Open(Comando);

        else if (aux[aux.length - 1].equals("RemoveSelectTotal"))       return Ambiente.removeSelectTotal(Comando);
        else if (aux[aux.length - 1].equals("RemoveSelectParcial"))     return Ambiente.removeSelectParcial(Comando);
        else if (aux[aux.length - 1].equals("addComp"))                 return Ambiente.addComp(Comando);
        else if (aux[aux.length - 1].equals("limpar"))                  return Ambiente.limpar(Comando);
        else if (aux[aux.length - 1].equals("variar"))                  return Ambiente.variar(Comando);

        else if (aux[aux.length - 1].equals("Otimizar"))                return Ambiente.otimizar(Comando);

        else return "Sem If direcionado!";

    }

}
