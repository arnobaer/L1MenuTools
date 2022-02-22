{# Template for signal type conditions like centrality #}
{# See in: L1Trigger/L1TNtuples/interface/L1AnalysisL1UpgradeDataFormat.h #}

{% block SignalsTemplate scoped %}
{% import 'macros.jinja2' as macros %}
{% set object = cond.getObjects()[0] %}
{% set prefix = object | getPrefix  %}
{% set signal_value = cond.getType() | getSignalValue  %}

bool
{{ cond.getName() }}
(L1Analysis::L1AnalysisL1UpgradeDataFormat* data)
{
  bool pass = false;
  {# Is signals in same bx? #}
  if (data->{{ prefix }}Bx.at(0) == {{ object.getBxOffset() }})
  {
    {# Is signal set? #}
    if (data->{{ prefix }}Type.at(0) == {{ signal_value }})
    {
      pass = true;
    }
  }
  return pass;
}
{% endblock SignalsTemplate %}
