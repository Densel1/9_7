#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    dataBase = new QSqlDatabase();
    //Объект QSqlQuery отвечает за формирование запросов к БД
    myQuery = new QSqlQuery();
    //QTableWidget необходим для формирования отображения ответа и передачи его в форму.
 //   myWidget = new QTableView();

}



DataBase::~DataBase()
{
    delete dataBase;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{

    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);

}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());

    ///Тут должен быть код ДЗ
    // Скорее всего не должен
    bool status;
    status = dataBase->open( );
    emit sig_SendStatusConnection(status);
}

/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{
    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();
}
/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
++++++++ * \return
 */
void DataBase::RequestToDB(QString request)
{

    ///Тут должен быть код ДЗ

    *myQuery = QSqlQuery(*dataBase);

    QSqlError err;
    qDebug() << "request: " << request;
    if(myQuery->exec(request) == false)
    {
        err = myQuery->lastError();}
    emit sig_SendStatusRequest(err);

}


void DataBase::ReadAnswerFromDB(int requestType)
{
    /*
     * Используем оператор switch для разделения запросов
    */
    switch (requestType) {
    //Для наших запросов вид таблицы не поменяетя. Поэтому бужет единый обработчик.
    case requestAllFilms:
    case requestComedy:
    case requestHorrors:
    {
        /*
         * Объект tableWidget представляет из себя таблицу, которую можно отобразить
         * в ПИ. Зfдадим ее параметры и заголовки. Мы получаем из БД название фильма, год выпуска и жанр
        */

        /* QSqlQuery
         * После запроса к БД ответ помещается в объект QSqlQuery.
         * Его необходимо считвать построчно. Для чтения ответа используется метод
         * next, он работает следующим образом: при вызове метода указатель перемещается
         * на следующую запись, в ответе. После получения ответа он указывает, если можно так
         * выразится на -1 запись. Т.е. при первом вызове метода next указатель переместится
         * на первую запись, а сам метод вернет true. В случае если указатель переместится
         * за диапазон записей метод вернет false. Также он вернет false в случае отсутствия
         * записей.
         * Каждая запись является стройкой прочитанной из БД. Для перемещения по столбцам
         * используется метод QSqlQuery value(индекс столбца).
        */
        /* QTableWidget
         * Необходимо сформировать таблицу, которую потом передадим в MainWindow для отображения
         * Для каждой записи прочитанной при помощи метода next будем добавлять строку в tableWidget
         * и заполнять значениями из ответа. Каждая ячейка таблицы является объектом класса
         * QTableWidgetItem. Т.е. для заполнения каждой ячеки мы должны создать экземпляр этого метода.
         * Значение ячейки в таблицк устанавливается при помощи метода setItem.
        */
        /*
         * После заполнения всей таблицы мы ее передаем в MainWindow при помощи сигнала.
        */

        QStringList hdrs;
        model = new QStandardItemModel();

        hdrs << "film_id"
             << "title"
             << "description"
             << "release_year"
             << "language_id"
             << "original_language_id"
             << "rental_duration"
             << "rental_rate"
             << "length"
             << "replacement_cost"
             << "rating"
             << "last_update"
             << "special_features"
             << "fulltext";

        model->setHorizontalHeaderLabels(hdrs);

        uint32_t conterRows = 0;
        QStandardItem* item = 0;
        while(myQuery->next()){
            QString str;
        //    myWidget->insertRow(conterRows);

            for(int i = 0; i<14; ++i){

                str = myQuery->value(i).toString();
                item = new QStandardItem(str);

                model->setItem(conterRows, i, item);

            }
            ++conterRows;
        }

        emit sig_SendDataFromDB(model, requestAllFilms);

        break;
    }

    default:
        break;
    }

}




/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}
