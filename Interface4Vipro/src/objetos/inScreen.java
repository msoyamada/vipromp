/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package objetos;

import java.awt.Image;
import java.awt.Point;
import java.util.Vector;
import javax.swing.ImageIcon;

/**
 *
 * @author alunospos
 */
public class inScreen {

    private Image imagem;
    private String caminhoI;
    private boolean isSelect;
    private String compName;
    private String compType;
    private Point local;

    private int Height;
    private int Width;

    private Vector caracs;
    private boolean hc;

    public inScreen(){

        imagem = new ImageIcon().getImage();
        isSelect = false;
        compName = "";
        local = new Point();

        this.Height = 0;
        this.Width = 0;

        hc = false;

    }

    public inScreen(Image imagem, boolean isSelect, String compName, String compType, String caminhoI, Point local, int Height, int Width){

        this.imagem = imagem;
        this.isSelect = isSelect;
        this.compName = compName;
        this.compType = compType;
        this.caminhoI = caminhoI;
        this.local = local;

        this.Height = Height;
        this.Width = Width;

        hc = false;

    }

    public inScreen(inScreen get) {

        this.imagem =       get.imagem;
        this.isSelect =     get.isSelect;
        this.compName =     get.compName;
        this.compType =     get.compType;
        this.caminhoI =     get.caminhoI;
        this.local =        get.local;

        this.Height =       get.Height;
        this.Width =        get.Width;

        hc = false;

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

    public String getCompName() {

        return compName;

    }

    public void setCompName(String compName) {

        this.compName = compName;

    }

    public String getCompType() {

        return compType;

    }

    public void setCompType(String compType) {

        this.compType = compType;

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

    }

    public int getHeight() {

        return this.Height;

    }

    public void setHeight(int Height) {

        this.Height = Height;

    }

    public int getWidth() {
        
        return this.Width;

    }

    public void setWidth(int Width) {

        this.Width = Width;

    }

    public void setCarac (Vector e){

        hc = true;
        this.caracs = new Vector(e);

    }

    public Vector getCarac (){

        if (hc) return this.caracs;
        else return null;

    }

    @Override
    public String toString(){

        String tudo = this.compName + ":&:" + this.compType + ":&:";

        return tudo;

    }

}
