#define AMOUNT_OF_COLUMNS 6

#include "MarksModel.h"

#include <algorithm>

#include "defines.h"

std::string formatCoord(double coord)
{
    std::stringstream strm;
    int degree = coord;
    int minutes = (coord - degree) * 60;
    double seconds = (((coord - degree) * 60) - minutes) * 60;
    strm << degree;
    if (degree < 10)
        strm << "ᵒ  ";
    else
        strm << "ᵒ ";
    strm << minutes;
    if (degree < 10)
        strm << "'  ";
    else
        strm << "' ";
    strm << seconds << "\"";
    return strm.str();
}

bool comp(QSharedPointer<DataMark> x1,QSharedPointer<DataMark> x2){return x1->getTime() < x2->getTime();}

MarksModel::MarksModel(const std::string &token, const WString &channel, WObject *parent)
    : WAbstractTableModel(parent)
{
    m_token = token;
    QSharedPointer<DataMarks> marks = common::DbSession::getInstance().getMarks();
    m_marks=makeHandle(new DataMarks());
    m_marks->clear();
    QSharedPointer<std::vector<QSharedPointer<common::User> > > users=common::DbSession::getInstance().getUsers();
    QSharedPointer<Channels> channels;

    for (int i = 0; i < users->size(); i++)
    {
        QSharedPointer<loader::User> user = users->at(i).dynamicCast<loader::User>();
        WString token = WString(user->getToken());
        if (token == WString(m_token))
        {
            channels = user->getSubscribedChannels();
            break;
        }
    }

    for (size_t y = 0; y < marks->size(); y++)
    {
        for (size_t i = 0; i < channels->size(); i++)
        {
            if (marks->at(y)->getChannel() == channels->at(i)) // ((*marks)[y]->getChannel() == (*channels)[i])
            {
                m_marks->push_back(marks->at(y)); // ((*marks)[y]);
            }
        }
    }

    std::sort(m_marks->begin(), m_marks->end(), comp);
}

int MarksModel::columnCount(const WModelIndex & parent) const
{
    return AMOUNT_OF_COLUMNS;
}

int MarksModel::rowCount(const WModelIndex & parent) const
{
    return m_marks->size();
}

boost::any MarksModel::data(const WModelIndex & index,
                              int role) const
{
    if (role == Wt::DisplayRole || role == Wt::EditRole || role == ToolTipRole)
    {
        switch (index.column())
        {
        case 0:
            return formatTime(m_marks->at(index.row())->getTime().getTime(),"%d %b %Y %H:%M:%S");
            // return m_marks->at(index.row())->getTime();
        case 1:
            return m_marks->at(index.row())->getChannel()->getName();
        case 2:
            return m_marks->at(index.row())->getDescription();
        case 3:
            return formatCoord(m_marks->at(index.row())->getLatitude());
        case 4:
            return formatCoord(m_marks->at(index.row())->getLongitude());
        case 5:
            return m_marks->at(index.row())->getUrl();
        default:
            return "undefined";
        }
        return "undefined";

    }
    else if (role == Wt::UrlRole)
    {
        if (index.column() == 5)
        {
            return m_marks->at(index.row())->getUrl();
        }
        return boost::any();
    }
    else
    {
        return boost::any();
    }
}

boost::any MarksModel::headerData(int section,
                                    Orientation orientation,
                                    int role) const
{
    if (section == 0)
        return "time";
    else if (section == 1)
        return "channel";
    else if (section == 2)
        return "description";
    else if (section == 3)
        return "latitude";
    else if (section == 4)
        return "longitude";
    else if (section == 5)
        return "url";
}

QSharedPointer<DataMarks> MarksModel::getMarks() const
{
    return m_marks;
}

#include <Wt/WFlags>

WFlags<ItemFlag> MarksModel::flags(const WModelIndex &index) const
{
    return WFlags<ItemFlag>();
}

void MarksModel::update()
{
    int curSize = m_marks->size();

    QSharedPointer<DataMarks> marks = common::DbSession::getInstance().getMarks();
    m_marks->clear();
    QSharedPointer<std::vector<QSharedPointer<common::User> > > users=common::DbSession::getInstance().getUsers();
    QSharedPointer<Channels> channels;

    for (int i = 0; i < users->size(); i++)
    {
        QSharedPointer<loader::User> user = users->at(i).dynamicCast<loader::User>();
        WString token = WString(user->getToken());
        if (token == WString(m_token))
        {
            channels = user->getSubscribedChannels();
            break;
        }
    }

    for (size_t y = 0; y < marks->size(); y++)
    {
        for (size_t i = 0; i < channels->size(); i++)
        {
            if (marks->at(y)->getChannel() == channels->at(i)) // ((*marks)[y]->getChannel() == (*channels)[i])
            {
                m_marks->push_back(marks->at(y)); // ((*marks)[y]);
            }
        }
    }

    std::sort(m_marks->begin(), m_marks->end(), comp);
int *a = NULL;
    if (m_marks->size() > curSize)
    {
        dataChanged().emit(createIndex(0, 0, a),
                           createIndex(m_marks->size() - 1,
                                       AMOUNT_OF_COLUMNS - 1,
                                       a));
    }
    else
    {
        dataChanged().emit(createIndex(0, 0, a),
                           createIndex(curSize - 1,
                                       AMOUNT_OF_COLUMNS - 1,
                                       a));
    }
}
