/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package objetos;

import java.awt.Image;
import java.awt.Point;

/**
 *
 * @author alunos
 */
public class CompDados {

    private Image imagem;
    private String caminhoI;
    private boolean isSelect;
    private Point local;
    private String compType;
    private String compName;

    private Point fuga1 = new Point();
    private Point fuga2 = new Point();
    private Point fuga3 = new Point();
    private Point fuga4 = new Point();

    public CompDados(){

        this.imagem = null;
        this.isSelect = false;
        this.caminhoI = null;
        this.local = null;

    }

    public CompDados(Image imagem, boolean isSelect, String name, String type, String caminhoI, Point local){

        this.imagem = imagem;
        this.isSelect = isSelect;
        this.caminhoI = caminhoI;
        this.local = local;
        this.compType = type;
        this.compName = name;

        fuga1.x = local.x + imagem.getWidth(null) / 2;
        fuga1.y = local.y + imagem.getHeight(null) - 5;
        if ((type.equals("processador"))) fuga1.y += +2;
        else if (type.equals("barramento") || ((type.equals("timer")))) fuga1.y += -2;
        else if (type.equals("memoria")) fuga1.y += +1;

        fuga2.x = local.x + imagem.getWidth(null) - 6;
        fuga2.y = local.y + imagem.getHeight(null) / 2;
        if (type.equals("processador") || type.equals("memoria")) fuga2.x += +2;
        else if (type.equals("barramento")) fuga2.x += +1;

        fuga3.x = local.x + imagem.getWidth(null) / 2;
        fuga3.y = local.y - 2;
        if ((type.equals("processador"))) fuga3.y = fuga3.y + 5;
        else if (type.equals("barramento")) fuga3.y += +8;
        else if (type.equals("bridge")) fuga3.y += +6;
        else if (type.equals("timer")) fuga3.y += +6;
        else if (type.equals("memoria")) fuga3.y += +5;

        fuga4.x = local.x + 3;
        fuga4.y = local.y + imagem.getHeight(null) / 2;
        if (type.equals("processador") || type.equals("memoria")) fuga4.x += -1;
        else if (type.equals("barramento")) fuga4.x += +1;

    }

    public Image getImagem() {

        return imagem;

    }

    public void setImagem(Image imagem) {

        this.imagem = imagem;

    }

    public boolean isSelect() {

        return isSelect;

    }

    public void setisSelect(boolean isSelect) {

        this.isSelect = isSelect;

    }

    public String getCaminho() {

        return caminhoI;

    }

    public void setCaminho(String caminhoI) {

        this.caminhoI = caminhoI;

    }

    public Point getLocal() {

        return local;

    }

    public void setLocal(Point local) {

        this.local = local;

        fuga1.x = local.x + imagem.getWidth(null) / 2;
        fuga1.y = local.y + imagem.getHeight(null) - 5;
        if ((this.compType.equals("processador"))) fuga1.y += +2;
        else if (this.compType.equals("barramento") || ((this.compType.equals("timer")))) fuga1.y += -2; 
        else if ((this.compType.equals("memoria"))) fuga1.y += +1;

        fuga2.x = local.x + imagem.getWidth(null) - 6;
        fuga2.y = local.y + imagem.getHeight(null) / 2;
        if (this.compType.equals("processador") || (this.compType.equals("memoria"))) fuga4.x += +2;
        else if (this.compType.equals("barramento")) fuga2.x += +1;

        fuga3.x = local.x + imagem.getWidth(null) / 2;
        fuga3.y = local.y - 2;
        if ((this.compType.equals("processador"))) fuga3.y = fuga3.y + 5;
        else if (this.compType.equals("barramento")) fuga3.y += +8;
        else if (this.compType.equals("bridge")) fuga3.y += +6;
        else if (this.compType.equals("timer")) fuga3.y += +6;
        else if (this.compType.equals("memoria")) fuga3.y += +5;

        fuga4.x = local.x + 3;
        fuga4.y = local.y + imagem.getHeight(null) / 2;
        if (this.compType.equals("processador") || (this.compType.equals("memoria"))) fuga4.x += -1;
        else if (this.compType.equals("barramento")) fuga4.x += +1;

    }

    public String getCompType() {

        return compType;

    }

    public void setCompType(String compType) {

        this.compType = compType;

    }

    public String getCompName() {

        return compName;

    }

    public void setCompName(String compName) {

        this.compName = compName;

    }

    public Point getFuga1(){

        return this.fuga1;

    }

    public Point getFuga2(){

        return this.fuga2;

    }

    public Point getFuga3(){

        return this.fuga3;

    }

    public Point getFuga4(){

        return this.fuga4;

    }

}
