{% extends "Condition.cc" %}

{% import 'macros.jinja2' as macros %}

{% set objects = cond.getObjects() %}
{% if overlap_removal %}
  {% set reference = objects[objects|length -1] %}
{% endif %}

{% set calo = objects[0] if objects[0].getType() in (tmEventSetup.Egamma, tmEventSetup.Tau, tmEventSetup.Jet) else objects[1] %}
{% set muon = objects[1] if objects[1].getType() == tmEventSetup.Muon else objects[0] %}
{% set prefixCalo = calo | getPrefix %}
{% set prefixMuon = muon | getPrefix %}

{% set etaScaleCalo = scaleMap | getScale(calo, tmGrammar.ETA) %}
{% set nEtaBitsCalo = etaScaleCalo.getNbits() %}
{% set phiScaleCalo = scaleMap | getScale(calo, tmGrammar.PHI) %}

{% set etaScaleMuon = scaleMap | getScale(muon, tmGrammar.ETA) %}
{% set nEtaBitsMuon = etaScaleMuon.getNbits() %}
{% set phiScaleMuon = scaleMap | getScale(muon, tmGrammar.PHI) %}

{% set LUTS = scaleMap | getLookUpTable(calo, muon) %}

{% set iPiMuon = (0.5*(phiScaleMuon.getMaximum() - phiScaleMuon.getMinimum())/phiScaleMuon.getStep()) | int -%}

{% block ConditionLogic %}
  {% if overlap_removal %}
    {{ macros.getReference(reference, tmEventSetup, nEtaBits) }}
  {% endif %}
  size_t nobj0 = 0;
  for (size_t ii = 0; ii < data->{{prefixCalo}}Bx.size(); ii++)
  {
    if (not (data->{{prefixCalo}}Bx.at(ii) == {{ calo.getBxOffset() }})) continue;
    nobj0++;
    {% if prefixCalo | isTau %}
      if (nobj0 > {{macros.getMaxTaus()}}) continue;
    {% endif %}
{{ macros.checkObjectIndex(calo, 'nobj0') }}
    {% if overlap_removal %}
      {{ macros.removeOverlap(cond, calo, 'ii', reference, tmEventSetup, scaleMap, iPi) }}
    {% endif %}
    {{ macros.getObjectCuts(prefixCalo, 'ii', calo, tmEventSetup, nEtaBitsCalo) }}
    size_t nobj1 = 0;
    for (size_t jj = 0; jj < data->{{prefixMuon}}Bx.size(); jj++)
    {
      if (not (data->{{prefixMuon}}Bx.at(jj) == {{ muon.getBxOffset() }})) continue;
      nobj1++;
  {{ macros.checkObjectIndex(muon, 'nobj1') }}
      {% if overlap_removal %}
        {{ macros.removeOverlap(cond, muon, 'jj', reference, tmEventSetup, scaleMap, iPi) }}
      {% endif %}
      {{ macros.getObjectCuts(prefixMuon, 'jj', muon, tmEventSetup, nEtaBitsMuon) }}
      {{ macros.getDifferentTypeCorrelationCuts(prefixCalo, prefixMuon, 'ii', 'jj', cond, tmEventSetup, LUTS, iPiMuon) }}
      pass = true;
      break;
    }
    if (pass) break;
  }
{% endblock ConditionLogic %}
