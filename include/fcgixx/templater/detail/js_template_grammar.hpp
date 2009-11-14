#pragma once


#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_confix.hpp>



namespace runpac { namespace fcgixx { namespace detail {


struct js_template_grammar : public boost::spirit::classic::grammar<js_template_grammar>
{

    typedef boost::function<void (const char *, const char *)> action_f;

    js_template_grammar(
         action_f& raw_action
        ,action_f& directive_action
        ,action_f& echo_action
        ,action_f& close_action
        ,action_f& statement_start_action
        ,action_f& else_action
        ,action_f& raw_escape_mode_action
        ,action_f& default_modifier_action
    )
    : raw_action(raw_action)
    , directive_action(directive_action)
    , echo_action(echo_action)
    , close_action(close_action)
    , statement_start_action(statement_start_action)
    , else_action(else_action)
    , raw_escape_mode_action(raw_escape_mode_action)
    , default_modifier_action(default_modifier_action)
    {
    }


    template <typename ScannerT> struct definition
    {
        definition(js_template_grammar const& self)
        {
            using namespace boost::spirit::classic;


            conf =
                *( raw[self.raw_action] | directive );

            open_directive =
                str_p("{%");

            close_directive =
                str_p("%}");

            raw =
                anychar_p - open_directive;

            directive =
                   open_directive
                >> inner_directive
                >> close_directive;

            inner_directive =
                  echo
                | statement;


            modifier_seq =
                  +(ch_p(':')
                >> (*(anychar_p - (ch_p('=')|ch_p(':')) ))[self.raw_escape_mode_action]);

            echo =
                   !modifier_seq
                >> ch_p('=')
                >> (*(anychar_p - close_directive))[self.echo_action];

            statement =
                   *space_p
                >> eps_p[self.statement_start_action]
                >> ( str_p("end")[self.close_action]
                   | (
                          !(str_p("else")[self.else_action])
                       >> (*(anychar_p - ( *space_p >> close_directive)))[self.directive_action]
                     )
                   )
                >> *space_p;
        }

        boost::spirit::classic::rule<ScannerT>
            conf, raw, directive, open_directive, close_directive, inner_directive,
            echo, statement, modifier_seq
            ;

        boost::spirit::classic::rule<ScannerT> const&  start() const { return conf; }
    };


    action_f& raw_action;
    action_f& directive_action;
    action_f& echo_action;
    action_f& close_action;
    action_f& statement_start_action;
    action_f& else_action;
    action_f& raw_escape_mode_action;
    action_f& default_modifier_action;

};


} } }// ns

