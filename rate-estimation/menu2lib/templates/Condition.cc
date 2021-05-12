bool
{{ cond.getName() }}
(L1Analysis::L1AnalysisL1UpgradeDataFormat* data)
{
  bool pass = false;

{% block ConditionLogic %}{% endblock %}

  return pass;
}
