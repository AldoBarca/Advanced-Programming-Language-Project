using LiveCharts;
using LiveCharts.Wpf;
using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Windows.Forms;
using LiveCharts.Defaults;
using System.Data.SqlClient;


namespace GUI
{
    public partial class FormGraficoLive : Form,IDataBaseReader
    {  
        
       
        //Campi ottenuti dal form precedente, contenenti il riferimento al form stesso e i dati scelti dall'utente.
        private Form _form_visualizzaVeicolo;
        private String _campo;
        private int _id_veicolo; 
        private double _frequenza_aggiornamento;
        private DateTime _tempo_inizio_visualizzazione;
        private DateTime _tempo_fine_visualizzazione;
        

        private LiveCharts.WinForms.CartesianChart _cartesianChart1; //Grafico cartesiano con valori ChartValues, è creato a run time, non è già presente nel file designer
        private Timer _timer;//Campo timer usata con l'evento tick per eseguire periodicamente un metodo
        private double _media;  
        private double _massimo;
        private double _somma_valori; //somma valori totali, usato per fare la media (sommavalori/numerovalori)
        private double _valore;  //singolo valore usato per prendere i dati dal db.
        private double _minimo;
        private bool _MinisInitialized; //bool che indica se il minimo è stato inizializzato o meno cosi da porlo uguale a valore che di fatto è il primo minimo)
        private bool _visualizza_dati_statistici; //bool per indicare se l'utente vuole vedere i dati statistici o meno.
        private ChartValues<ObservablePoint> _values; //Grafico dove si inseriscono i dati
        private Queue<double> _coda_elementi_db; //coda fatta dagli elementi (valori) presi dal db, forniranno i valori dell'asse y del grafico
        private Queue<DateTime> _coda_timestamp;//coda fatta dai timestamp (valori) presi dal db,forniranno i valori dell'asse x del grafico

        private double _timestamp_in_double; //campo usato in combinazione con timestamp formattato per trasformare in double e visualizzarli nel grafico
        private string _timestamp_formattato;
     
        

        public FormGraficoLive(Form form, string stringa, int id,double refresh_rate,DateTime tempo_inizio,DateTime tempo_fine)
        {

            // fase inizializzazione campi, in base a criteri logici, ad esempio massimo=0 finche non ve n'è uno,
            // oppure dati passati al costruttore dall'utente per campi come tempo_inizio_visualizzazione.
            InitializeComponent(); 
            _timer = new Timer();


            _tempo_inizio_visualizzazione = tempo_inizio;
            _tempo_fine_visualizzazione = tempo_fine;



            _frequenza_aggiornamento =refresh_rate;
            _coda_elementi_db = new Queue<double>();
            _coda_timestamp = new Queue<DateTime>();    

            _cartesianChart1 = new LiveCharts.WinForms.CartesianChart();
            _cartesianChart1.Width = 1250;
            _cartesianChart1.Height = 450;
            Controls.Add(_cartesianChart1);

            _MinisInitialized = false;
            _visualizza_dati_statistici = true;
            _form_visualizzaVeicolo = form;
            _id_veicolo = id;
            _campo = stringa;
            _massimo = 0;
            _media = 0;
            _somma_valori = 0;
            _values = new ChartValues<ObservablePoint>();
            


            //impostazione grafico, in formato Collezione di serie
            _cartesianChart1.Series = new SeriesCollection
            {
                new LineSeries 
                {
                    Title = "Live Data",
                    Values = _values
                }
            };


            //inizializzazione assi cartesiani 
            _cartesianChart1.AxisX.Add(new Axis
            {
                Title = "Time",
                LabelFormatter = value => value.ToString("N")
            });
            _cartesianChart1.AxisY.Add(new Axis
            {
                Title = _campo,
                LabelFormatter = value => value.ToString("N")
            });


            //inizializzazione dei valori massimi e minimi visualizzabili sull'asse x (dei tempi) sul grafico, settato secondo la
            //finestra scelta dall'utente.
            _timestamp_formattato = _tempo_fine_visualizzazione.ToString("yyyyMMddHHmmss");
            _timestamp_in_double = double.Parse(_timestamp_formattato);
            _cartesianChart1.AxisX[0].MaxValue = _timestamp_in_double;

            _timestamp_formattato = _tempo_inizio_visualizzazione.ToString("yyyyMMddHHmmss");
            _timestamp_in_double = double.Parse(_timestamp_formattato);
            _cartesianChart1.AxisX[0].MinValue = _timestamp_in_double;



            int i = 0; // la i serve per calcolare la media, servono il numero di elementi
            Prendi_dati_Db(); //Metodo che prende  i valori dei dati di un campo e il corrispettivo timestamp  dal database e li inserisce
                              //nella coda_elementi_db e coda_timestamp. 
            Console.WriteLine(_coda_elementi_db);
            while(_coda_elementi_db.Count>0)
            {   //Qui vengono presi i dati dalle code e con i dati si calcolano i dati statistici e poi si mettono nel grafico
                _timestamp_formattato = _coda_timestamp.Dequeue().ToString("yyyyMMddHHmmss");
                _timestamp_in_double = double.Parse(_timestamp_formattato);

                _valore = _coda_elementi_db.Dequeue();
               


                if (_MinisInitialized==false)
                {
                    _MinisInitialized = true;
                    _minimo = _valore;


                }

                if (_valore>_massimo) _massimo=_valore;
                if(_valore<_minimo) _minimo=_valore;
                _somma_valori += _valore;


                _values.Add(new ObservablePoint( _timestamp_in_double,_valore));
                Console.WriteLine(_values);

            
                i++;
                _cartesianChart1.Update();

            }
            _media=_somma_valori/(i);
            _media = Math.Round(_media, 3);
            textBox1.Text = _media.ToString();
            textBox2.Text = _massimo.ToString();
            textBox3.Text = _minimo.ToString();


            //Associazione del metodo Update_Grafico all'evento Tick di un oggetto timer. Il rate di tick è scelto dall'utente nel form precendente
            _timer.Interval = (int)_frequenza_aggiornamento*1000; // frequenza di aggiornamento dati
     
            _timer.Tick += Update_grafico;
            
            _timer.Start();

        }


        public void Prendi_dati_Db()
        {
            string connString = "server=localhost;port=3306;database=apl;uid=root;password=";
            using (MySqlConnection connection = new MySqlConnection(connString))
                try
                {
          
                    connection.Open();

                    string query = "SELECT p.valore, p.timestamp FROM pacchetto p join legenda l on p.cod_legenda=l.id  where p.cod_veicolo= " + _id_veicolo.ToString() 
                            +" AND l.nome= '"+_campo+ "' AND p.timestamp >= '" + _tempo_inizio_visualizzazione.ToString("yyyy-MM-dd HH:mm:ss") +
                            "' AND p.timestamp <= '"
                            + _tempo_fine_visualizzazione.ToString("yyyy-MM-dd HH:mm:ss") + "' order by p.timestamp ASC";
                   
                    
                    Console.WriteLine(query);
                    MySqlCommand command = new MySqlCommand(query, connection);
                    using (MySqlDataReader reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            _coda_elementi_db.Enqueue(reader.GetDouble(0));
                            _coda_timestamp.Enqueue(reader.GetDateTime(1));
                        }
                    }
                }
                catch (SqlException ex)
                {
                    if (ex.Number == -1)
                    {
                        MessageBox.Show("Errore di connessione al database, riprovare più tardi");
                        Console.WriteLine(ex.Message);
                        _form_visualizzaVeicolo.Show();
                        this.Close();
                    }
                    else
                    {
                        MessageBox.Show("Errore SQL generico, riprovare più tardi");
                        Console.WriteLine(ex.Message);
                        _form_visualizzaVeicolo.Show();
                        this.Close();
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    MessageBox.Show("Errore nella visualizzazione dei dati, riprova più tardi!");
                    this.Close();
                    _form_visualizzaVeicolo.Show();
                }
        }

        private void Update_grafico(object sender, EventArgs e)
        {
            _massimo = 0;
            _media = 0;
            _somma_valori = 0;
            Prendi_dati_Db();
            int i = 0;
            _values = new ChartValues<ObservablePoint>();
            while (_coda_elementi_db.Count > 0)
            {
                _valore = _coda_elementi_db.Dequeue();

                _timestamp_formattato = _coda_timestamp.Dequeue().ToString("yyyyMMddHHmmss");
                _timestamp_in_double = double.Parse(_timestamp_formattato);


                if (_valore > _massimo) _massimo = _valore;
                if (_valore < _minimo) _minimo = _valore;
                _somma_valori += _valore;


                _values.Add(new ObservablePoint(_timestamp_in_double, _valore));
                Console.WriteLine(_values);
                
                i++;

            }
            _media = _somma_valori / (i);
            _media = Math.Round(_media, 3);
            textBox1.Text = _media.ToString();
            textBox2.Text = _massimo.ToString();
            textBox3.Text = _minimo.ToString();

            _cartesianChart1.Series[0].Values = _values;                
        }


        private void button1_Click_1(object sender, EventArgs e)
        {
            _form_visualizzaVeicolo.Show();
            this.Close();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //bottone per visualizzare/nascondere i dati statistici
            if (_visualizza_dati_statistici) {
                _visualizza_dati_statistici=false;

                textBox1.Hide();
                label1.Hide();
                textBox2.Hide();
                label2.Hide();
                textBox3.Hide();
                label3.Hide();
                button4.Text = "Visualizza Dati Statistici";
            }
            else
            {
                _visualizza_dati_statistici = true;
                textBox1.Show();
                label1.Show();
                textBox2.Show();
                label2.Show();
                textBox3.Show();
                label3.Show();
                button4.Text = "Nascondi Dati Statistici";

            }
        }
    }
}

