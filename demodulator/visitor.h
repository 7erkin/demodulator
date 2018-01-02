#ifndef VISITOR_H
#define VISITOR_H

class visitor
{
public:
    visitor();
    virtual ~visitor() = default;
    virtual void visit() = 0;
    virtual void visit() = 0;
};

class read_from_file : public visitor{
public:
};
class read_from_udp_socket : public visitor{
public:
};
#endif // VISITOR_H
