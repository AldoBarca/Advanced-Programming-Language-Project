using System;
using System.Windows.Forms;

namespace GUI
{
    public partial class FormFirstPage : Form
    {   // Form iniziale della GUI. Molto semplice, chiede all'utente se vuole iniziare la visualizzazione oppure se chiudere l'applicazione.
        

        private FormVisualizzaVeicolo _formVisualizzaVeicoloEsistente;
   
        public FormFirstPage()
        {   
            InitializeComponent();
          
        }

        

        private void button2_Click(object sender, EventArgs e)
        {   
           
            DialogResult dr=MessageBox.Show("Vuoi chiudere l'applicazione?", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question);

            if (dr==DialogResult.Yes)
            {
                this.Close();
                
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {   //Se l'utente vuole andare avanti crea un'istanza della classe FormVisualizzaVeicolo, di fatto crea un form, che mostra e nasconde questo
            _formVisualizzaVeicoloEsistente =new FormVisualizzaVeicolo(this);
            if (!_formVisualizzaVeicoloEsistente.IsDisposed)
            {
                this.Hide();
                this.CenterToScreen(); //metodo che centralizza il form
                _formVisualizzaVeicoloEsistente.Show();
               
                
            }
        }
    }
}
