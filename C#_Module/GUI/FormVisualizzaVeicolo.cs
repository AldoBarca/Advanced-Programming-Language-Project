using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data.SqlClient;
using MySql.Data.MySqlClient;


namespace GUI
{
    public partial class FormVisualizzaVeicolo : Form, IDataBaseReader
    {
        
        private Form _form_grafico; // Form successivo che l'utente visualizzerà dopo aver selezionato i campi di proprio interesse
        private Form _firstPage; //Form precedente, gli viene passato nel costruttore cosi che se l'utente vuole tornare indietro
        //verrà mostrato tramite il metodo .Show(), attualmente è nascosto
        private List<int> _lista_id;  //Lista degli id da visualizzare per l'utente, presi da db
        private String _dato_da_visualizzare;
        private List<String> _campi; //Lista dei campi, presa da db e mostrata all'utente per poter scegliere.
        private String _campo;






        public FormVisualizzaVeicolo(FormFirstPage form)
        {


            _lista_id = new List<int>();
            _campi = new List<String>();
            _firstPage = form;
            InitializeComponent();
            //Impedisce all'utente di scrivere altri possibili campi oltre quelli selezionati. Blocca la SQL Injection
            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBox2.DropDownStyle = ComboBoxStyle.DropDownList;



            //Formato per il datapicker, controllo che si occupa di far scegliere all'utente un data+orario.
            dateTimePicker1.Format = DateTimePickerFormat.Custom;
            dateTimePicker1.CustomFormat = "yyyy-MM-dd HH:mm:ss";
            dateTimePicker2.Format = DateTimePickerFormat.Custom;
            dateTimePicker2.CustomFormat = "yyyy-MM-dd HH:mm:ss";

            //Si prende da db la lista dei campi
            Prendi_dati_Db();


            //Si aggiungono i campi alla checkBox per farne scegliere uno all'utente
            foreach (String campo in _campi)
            {
                comboBox2.Items.Add(campo);
            }


        }


        public void Prendi_dati_Db()
        {
            String connString = "server=localhost;port=3306;database=apl;uid=root;password=";
            using (MySqlConnection connection = new MySqlConnection(connString))
                try
                {

                    connection.Open();

                    string query = "SELECT nome FROM legenda";
                    string query2 = "SELECT * FROM veicolo";


                    MySqlCommand command = new MySqlCommand(query, connection);
                    using (MySqlDataReader reader = command.ExecuteReader())
                    {

                        while (reader.Read())
                        {
                            _campo = reader.GetString(0);
                            if (!_campi.Contains(_campo))  
                            {
                                _campi.Add(_campo);

                            }

                        }
                    }


                    MySqlCommand command2 = new MySqlCommand(query2, connection);
                    using (MySqlDataReader reader = command2.ExecuteReader())
                    {
                        int i = 1;
                        while (reader.Read())
                        {
                            int id = reader.GetInt32(0);
                            if (!_lista_id.Contains(id))
                            {
                                _lista_id.Add(id);
                                comboBox1.Items.Add("Veicolo numero:" + id.ToString());
                            }

                            i++;


                        }
                    } 
                }
                catch (SqlException ex)
                {
                    if (ex.Number == -1)
                    {
                        MessageBox.Show("Errore di connessione al database, riprovare più tardi");
                        Console.WriteLine(ex.Message);
                        _firstPage.Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Errore SQL generico, riprovare più tardi");
                        Console.WriteLine(ex.Message);
                        _firstPage.Show();
                        this.Close();
                    }
                }
                catch (Exception ex)
                {

                    MessageBox.Show("Ci scusiamo per il disagio ma è impossibile gestire la richiesta attualmente, riprovare più tardi");
                    Console.WriteLine("Errore durante l'esecuzione della query: " + ex.Message);
                    _firstPage.Show();
                    this.Close();
                }   
        }



        private void button2_Click(object sender, EventArgs e)
        {
           //Se clicchi sul bottone 2 torni al form precedente, chiudendo questo.
            _firstPage.Show();
            this.Close();
        }



        private void button1_Click(object sender, EventArgs e)
        {
            //Vai al form di visualizzazione grafico live, passando tutti i dati inseriti dall'utente


            double frequenza_aggiornamento;




            //Controlli per verificare che l'utente abbia selezionato tutto
            if (comboBox1.SelectedItem == null)
            {
                MessageBox.Show("Selezionare un veicolo di cui visualizzare i dati.");
            }
            else if (comboBox2.SelectedItem == null)
            {
                MessageBox.Show("Selezionare i campi da visualizzare per il veicolo selezionato.");
            }
            else if (double.TryParse(textBox1.Text, out frequenza_aggiornamento) == false)
            {
                MessageBox.Show("Selezionare una frequenza di aggiornamento dati numerica (double accettabili).");
            }
            else if(dateTimePicker1.Value > dateTimePicker2.Value)
            {

                MessageBox.Show("Selezionare un orario di inizio precendente a quello di fine ");
                
            }
            else
            {
                int id_veicolo = 0;
                for (int i = 0; i < comboBox1.Items.Count; i++)
                {
                    if (comboBox1.Items[comboBox1.SelectedIndex].ToString() == "Veicolo numero:" + _lista_id[i].ToString())
                    {
                        id_veicolo = _lista_id[i];
                    }
                }

                _dato_da_visualizzare = comboBox2.SelectedItem.ToString();

               

                //Verifica che vi siano risultati per la scelta dell'utente, poi l'utente potrà scegliere in caso non ve ne siano
                //se visualizzare un grafico vuoto o cambiare dati.
                string connString = "server=localhost;port=3306;database=apl;uid=root;password=";
                using (MySqlConnection connection = new MySqlConnection(connString)) //chiama: oggettodefinito.Dispose() a fine blocco.
                    //non servono cosi usare finalizers o dispose manuali.
                    try
                    {   
                        connection.Open();

                        string query= "SELECT * FROM pacchetto p join legenda l on p.cod_legenda=l.id  where p.cod_veicolo= " + id_veicolo
                            + " AND l.nome= '" + _dato_da_visualizzare + "' AND p.timestamp >= '" + dateTimePicker1.Value.ToString("yyyy-MM-dd HH:mm:ss") +
                            "' AND  p.timestamp <= '"
                            + dateTimePicker2.Value.ToString("yyyy-MM-dd HH:mm:ss") + "' order by p.timestamp ASC";

                        //string query = "SELECT * FROM pacchetto where cod_veicolo= " + id_veicolo;
                        MySqlCommand command = new MySqlCommand(query, connection);
                        using (MySqlDataReader reader = command.ExecuteReader())
                        {
                           
                            if (reader.Read() == false) 
                            {

                                DialogResult dr = MessageBox.Show("Non sono stati trovati dati per il veicolo selezionato, vuoi comunque visualizzare il grafico?" +
                                    "", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question);

                                if (dr == DialogResult.No)
                                {
                                    return;
                                }
                            }
                        }
                    }
                    catch (SqlException ex)
                    {
                        
                        if (ex.Number == -1)
                        {
                            MessageBox.Show("Errore di connessione al database, riprovare più tardi");
                            Console.WriteLine(ex.Message);
                            _firstPage.Show();
                            this.Close();
                        }
                        else
                        {
                            MessageBox.Show("Errore SQL generico, riprovare più tardi");
                            Console.WriteLine(ex.Message);
                            _firstPage.Show();
                            this.Close();
                        }
                    }
                    catch (Exception ex)
                    {

                        MessageBox.Show("Ci scusiamo per il disagio ma è impossibile gestire la richiesta attualmente, riprovare più tardi");
                        Console.WriteLine("Errore durante l'esecuzione della query: " + ex.Message);
                        _firstPage.Show();
                        this.Close();
                    }
             
                this.Hide();
               
                _form_grafico = new FormGraficoLive(this, _dato_da_visualizzare, id_veicolo, frequenza_aggiornamento, dateTimePicker1.Value
                , dateTimePicker2.Value);

                if (!_form_grafico.IsDisposed) {
                    this.CenterToScreen();
                    _form_grafico.Show();

                }
            }
        }
    }
}

